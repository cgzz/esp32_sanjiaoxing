#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "my_web.h"

// 变量暴露
// 文件系统
#define FSYS LittleFS
// 以下是参数定义

// web刷新限制
#define REFRESH_RATE_DEF 10
#define REFRESH_RATE_MAX 60
#define REFRESH_RATE_MIN 1
// 异步服务器对象
extern AsyncWebServer server;
extern AsyncWebSocket ws;

// ===== 业务桥接：对外暴露的回调指针类型定义 =====
typedef uint32_t (*TelemetryFn)();
typedef void (*PidSetFn)(JsonObject param);
typedef JsonDocument (*PidGetFn)();
typedef void (*TestModeFn)(bool enabled, int mode, float value);
typedef void (*JoystickFn)(float x, float y, float a);
// ===== 业务桥接：对外暴露的回调指针 =====
extern PidSetFn cb_pid_set_fn;
extern PidGetFn cb_pid_get_n;
extern TelemetryFn cb_telem_fn;
extern TestModeFn cb_testmode_fn;
extern JoystickFn cb_joystick_fn;

// 数据更新
uint32_t cb_telemetry();
// fs函数
static String contentType(const String &path);

// webtool函数
void wsSendTo(AsyncWebSocketClient *c, const JsonDocument &doc);
void my_wsheart();
bool wsCanBroadcast();
void wsBroadcast(const JsonDocument &doc);
bool handleFileRead(AsyncWebServerRequest *req, String path);

// tool函数
float my_math_deadband(float value, float deadband);
float my_math_limit(float value, float min, float max);
void rtrim_inplace(char *s);