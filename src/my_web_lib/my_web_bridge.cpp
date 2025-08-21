#include "my_web_config.h"
#include "my_config.h"
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
const char *keys[13] = {"angP", "angI", "angD", "spdP", "spdI", "spdD", "posP", "posI", "posD", "yawP", "yawI", "yawD", "zero"};

// UI 配置：图表与滑条信息
struct ChartConfig
{
    const char *title;
    const char *legend[3];
};

struct SliderGroup
{
    const char *group;
    const char *names[3];
};

static const ChartConfig chart_cfg[3] = {
    {"姿态角", {"Pitch", "Roll", "Yaw"}},
    {"电机电流", {"Left", "Right", "Total"}},
    {"电池状态", {"Voltage", "Current", "Temp"}},
};

static const SliderGroup slider_cfg[4] = {
    {"Angle PID", {"P", "I", "D"}},
    {"Speed PID", {"P", "I", "D"}},
    {"Position PID", {"P", "I", "D"}},
    {"Yaw PID", {"P", "I", "D"}},
};

// UI 配置 JSON 打包
JsonDocument cb_ui_config()
{
    StaticJsonDocument<512> doc;
    JsonArray charts = doc.createNestedArray("charts");
    for (const auto &c : chart_cfg)
    {
        JsonObject o = charts.createNestedObject();
        o["title"] = c.title;
        JsonArray l = o.createNestedArray("legends");
        for (int i = 0; i < 3; ++i)
            l.add(c.legend[i]);
    }
    JsonArray sliders = doc.createNestedArray("sliders");
    for (const auto &sg : slider_cfg)
    {
        JsonObject g = sliders.createNestedObject();
        g["group"] = sg.group;
        JsonArray n = g.createNestedArray("names");
        for (int i = 0; i < 3; ++i)
            n.add(sg.names[i]);
    }
    doc["type"] = "ui_config";
    return doc;
}

// 12+1 路遥测数据（自定义映射：电流/角速度/位置/温度...）
uint32_t cb_telemetry()
{
    StaticJsonDocument<320> doc;
    uint32_t hz = telem_hz ? telem_hz : 1;
    uint32_t dt_ms = 1000 / hz;
    // 组包 -> 广播
    doc["type"] = "telemetry";
    doc["fallen"] = send_fall;
    JsonArray arr = doc.createNestedArray("d");
    int limit = charts_send ? 12 : 3;
    for (int i = 0; i < limit; i++)
        arr.add(send_msg[i]);
    wsBroadcast(doc);
    return dt_ms; // 返回下次采样间隔（ms）
}
// PID 设置（顺序：角度P/I/D，速度P/I/D，位置P/I/D）
void cb_pid_set(JsonObject param)
{
    if (!param.isNull())
        for (int i = 0; i < 13; i++)
            pid_cache[i] = param[keys[i]] | pid_cache[i]; // 没有就用原始数据
    swing_up_voltage = pid_cache[0];
    //  = pid_cache[1];
    //  = pid_cache[2];
    //  = pid_cache[3];
    //  = pid_cache[4];
    //  = pid_cache[5];
    //  = pid_cache[6];
    //  = pid_cache[7];
    //  = pid_cache[8];
    //  = pid_cache[9];
    //  = pid_cache[10];
    //  = pid_cache[11];
    target_angle = pid_cache[12];
}
// PID 读取
JsonDocument cb_pid_get()
{
    StaticJsonDocument<320> out;
    JsonObject pr = out.createNestedObject("param");
    out["type"] = "pid";
    pid_cache[0] = swing_up_voltage;
    // pid_cache[1] = 0;
    // pid_cache[2] = 0;
    // pid_cache[3] = 0;
    // pid_cache[4] = 0;
    // pid_cache[5] = 0;
    // pid_cache[6] = 0;
    // pid_cache[7] = 0;
    // pid_cache[8] = 0;
    // pid_cache[9] = 0;
    // pid_cache[10] = 0;
    // pid_cache[11] = 0;
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