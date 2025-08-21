#include "my_web_config.h"
#include "my_control.h"

bool robot_run = false;
bool charts_send = true;
bool fallcheck_enable = true;

bool testmode_enabled = false;
int testmode_motor_mode = 0;
float testmode_value = 0;

float joyY = 0;
float joyX = 0;
float joyA = 0;

int telem_hz = 10;

bool send_fall = false;
float send_msg[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float pid_cache[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
const char *keys[13] = {"key01", "key02", "key03", "key04", "key05", "key06", "key07", "key08", "key09", "key10", "key11", "key12", "key13"};
// ui标签
ChartConfig chart_cfg[3] = {
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
};

SliderGroup slider_cfg[4] = {
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
};

void my_web_ui_init(SliderGroup slider_group[4], ChartConfig chart_config[3])
{
    for (int i = 0; i < 3; i++)
        chart_cfg[i] = chart_config[i];
    for (int i = 0; i < 4; i++)
        slider_cfg[i] = slider_group[i];
}
// ========== UI 配置 JSON 打包（仅首连时发送一次） ==========
JsonDocument cb_ui_config()
{
    // StaticJsonDocument<512> 容量偏小，容易截断；改为动态并给足余量
    DynamicJsonDocument doc(1536);

    // 把 type 放最前，前端能更快分发；哪怕极端截断也更易诊断
    doc["type"] = "ui_config";

    // charts
    JsonArray charts = doc.createNestedArray("charts");
    for (const auto &c : chart_cfg)
    {
        JsonObject o = charts.createNestedObject();
        o["title"] = c.title;
        JsonArray l = o.createNestedArray("legends");
        for (int i = 0; i < 3; ++i)
            l.add(c.legend[i]);
    }

    // sliders
    JsonArray sliders = doc.createNestedArray("sliders");
    for (const auto &sg : slider_cfg)
    {
        JsonObject g = sliders.createNestedObject();
        g["group"] = sg.group;
        JsonArray n = g.createNestedArray("names");
        for (int i = 0; i < 3; ++i)
            n.add(sg.names[i]);
    }

    doc.shrinkToFit(); // 发送前收紧空间，减轻带宽
    return doc;
}

// 12+1 路遥测数据（自定义映射：电流/角速度/位置/温度...）
uint32_t cb_telemetry()
{
    StaticJsonDocument<320> doc;
    uint32_t hz = telem_hz ? telem_hz : 1;
    uint32_t dt_ms = 1000 / hz;

    doc["type"] = "telemetry";
    doc["fallen"] = send_fall;

    JsonArray arr = doc.createNestedArray("d");
    int limit = charts_send ? 12 : 3; // 若前端关闭图表推送，仅发 3 路减载
    for (int i = 0; i < limit; i++)
        arr.add(send_msg[i]);

    wsBroadcast(doc);
    return dt_ms; // 返回下次采样间隔（ms）
}

// PID 设置（顺序：角度P/I/D，速度P/I/D，位置P/I/D，航向P/I/D，zero）
void cb_pid_set(JsonObject param)
{
    if (!param.isNull())
        for (int i = 0; i < 13; i++)
            pid_cache[i] = param[keys[i]] | pid_cache[i]; // 没有就用原始数据

    swing_up_voltage = pid_cache[0];
    // 其他 PID 参数在你接入后逐个赋值
    target_angle = pid_cache[12];
}

// PID 读取
JsonDocument cb_pid_get()
{
    StaticJsonDocument<320> out;
    JsonObject pr = out.createNestedObject("param");
    out["type"] = "pid";

    pid_cache[0] = swing_up_voltage;
    pid_cache[12] = target_angle;

    for (int i = 0; i < 13; i++)
        pr[keys[i]] = pid_cache[i];

    return out;
}

// 测试模式
void cb_testmode(bool enabled, int mode, float value)
{
    testmode_enabled = enabled, testmode_value = value, testmode_motor_mode = mode;
}

// 摇杆
void cb_joystick(float x, float y, float a)
{
    joyX = my_math_deadband(my_math_limit(x, -1.0f, 1.0f), 0.02f);
    joyY = my_math_deadband(my_math_limit(y, -1.0f, 1.0f), 0.02f);
    joyA = my_math_deadband(my_math_limit(a, -1.0f, 1.0f), 0.02f);
}

// ===== 回调指针定义 =====
PidSetFn cb_pid_set_fn = cb_pid_set;
PidGetFn cb_pid_get_n = cb_pid_get;
TelemetryFn cb_telem_fn = cb_telemetry;
TestModeFn cb_testmode_fn = cb_testmode;
JoystickFn cb_joystick_fn = cb_joystick;
