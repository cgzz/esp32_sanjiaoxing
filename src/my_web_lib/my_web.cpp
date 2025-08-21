#include "my_web_config.h"
// 运行态与遥测频率（Web 层本地状态）
// ======================= 内部状态 =======================
// Web/WS 服务实例（仅本翻译单元可见）
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
// ======================= 遥测组包 & 广播 =======================

// 事件处理
// 连接事件
void we_evt_connect(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    StaticJsonDocument<96> ack;
    ack["type"] = "info";
    ack["text"] = "connected";
    wsSendTo(c, ack);
}
// 断联事件
void we_evt_disconnect(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // TODO
}
// 消息事件
void ws_evt_data(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // 尝试解析 JSON 指令
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    StaticJsonDocument<320> doc;
    DeserializationError e = deserializeJson(doc, data, len);
    const char *typeStr = doc["type"] | "";
    // 无数据则提前结束处理
    if (!(info->final && info->index == 0 && info->len == len))
        return; // 仅处理完整帧
    if (info->opcode != WS_TEXT)
        return; // 仅处理文本帧
    if (e)
        return; // 非 JSON 可忽略
    if (!*typeStr)
        return; // 无 type 字段可忽略
    // ===========逻辑处理区域===========
    // 1) 设置遥测频率（上限锁 60Hz，避免队列堆积）
    if (!strcmp(typeStr, "set_rate"))
        telem_hz = my_math_limit(doc["hz"] | REFRESH_RATE_DEF, REFRESH_RATE_MIN, REFRESH_RATE_MAX);
    // 2) 运行开关（只影响执行器；不影响遥测是否发送）
    else if (!strcmp(typeStr, "toggle_run"))
        robot_run = doc["running"] | false; // 默认关闭
    // 3) 图表推送开关
    else if (!strcmp(typeStr, "charts_on"))
        charts_send = doc["on"] | true; // 默认打开
    // 4) 特殊状态检测开关
    else if (!strcmp(typeStr, "fall_detect"))
        fallcheck_enable = doc["enable"];
    // 5) 姿态零偏
    else if (!strcmp(typeStr, "att_zero"))
        return; // TODO 姿态恢复代码
    // 6) 摇杆
    else if (!strcmp(typeStr, "joy"))
        cb_joystick_fn(doc["x"] | 0.0f, doc["y"] | 0.0f, doc["deg"] | 0.0f);
    // 7) 设置 PID
    else if (!strcmp(typeStr, "set_pid"))
        cb_pid_set_fn(doc["param"].as<JsonObject>());
    // 8) 读取 PID（回填给前端）
    else if (!strcmp(typeStr, "get_pid"))
        wsSendTo(c, cb_pid_get_n());
    // 9) 测试模式：仅解析并回调（不保状态/不实现逻辑）
    else if (!strcmp(typeStr, "test_mode"))
        cb_testmode_fn(doc["enabled"].as<bool>() | false, doc["mode"] | 0, doc["value"] | 0.0f);
}
// ping/pong事件
void ws_evt_pong(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // TODO
}
// 错误事件
void ws_evt_error(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    // TODO
}
// ======================= WebSocket 事件处理 =======================
static void onWsEvent(AsyncWebSocket *s, AsyncWebSocketClient *c, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        we_evt_connect(s, c, type, arg, data, len);
        break;
    case WS_EVT_DISCONNECT:
        we_evt_disconnect(s, c, type, arg, data, len);
        break;
    case WS_EVT_DATA:
        ws_evt_data(s, c, type, arg, data, len);
        break;
    case WS_EVT_PONG:
        ws_evt_pong(s, c, type, arg, data, len);
        break;
    case WS_EVT_ERROR:
        ws_evt_error(s, c, type, arg, data, len);
        break;
    default:
        break;
    }
}

// ======================= HTTP 处理器 =======================
static void handleApiState(AsyncWebServerRequest *req)
{
    JsonDocument d;
    String s;
    d["running"] = true;
    d["hz"] = telem_hz;
    serializeJson(d, s);
    req->send(200, "application/json; charset=utf-8", s);
}

static void handleRootRequest(AsyncWebServerRequest *req)
{
    if (!handleFileRead(req, "/"))
        req->send(404, "text/plain; charset=utf-8", "home4.html not found");
}

static void handleNotFound(AsyncWebServerRequest *req)
{
    if (!handleFileRead(req, req->url()))
        req->send(404, "text/plain; charset=utf-8", "404 Not Found");
}

// ======================= 路由 & 初始化入口 =======================
void my_web_asyn_init()
{
    if (!FSYS.begin(true)) // 1) 文件系统
        Serial.println("[WEB] LittleFS mount failed (formatted?)");
    ws.onEvent(onWsEvent); // 3) WebSocket
    server.addHandler(&ws);
    server.on("/api/state", HTTP_GET, handleApiState); // 4) 基础 API
    server.on("/", HTTP_GET, handleRootRequest);       // 5) 静态文件
    server.onNotFound(handleNotFound);
    server.begin(); // 6) 启动 HTTP
}

// 数据更新
uint32_t my_web_data_update()
{
    return cb_telemetry();
    // 其他数据更新
}