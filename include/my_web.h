#ifndef MY_WEB_H
#define MY_WEB_H
#include "mpu6050_hal.h"
// wifi定义
#define SSID "BIEGUOQU"
#define PASSWORD "qazwsxedcr"
// 遥测数据发送 变量暴露 滑条读取和发送内容
extern bool send_fall;
extern float send_msg[12];  // 需要自行写入数据
extern float pid_cache[13]; // 需要自行同步数据
// 机器人运行指示位
extern bool robot_run;
extern bool charts_send;
extern bool fallcheck_enable;
// 摇杆控制
extern float joyY;
extern float joyX;
extern float joyA;
// 遥测频率
extern int telem_hz;
// 测试模式
extern bool testmode_enabled;
extern int testmode_motor_mode;
extern float testmode_value;

// 函数定义
void my_web_asyn_init();

void my_wifi_init();

uint32_t my_web_data_update();

// void my_wsheart(); 心跳函数

#endif // MY_WEB_H
