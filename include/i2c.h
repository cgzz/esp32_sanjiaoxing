// I2C/系统旧接口声明集合：保留历史项目中的辅助函数原型（WiFi打印、角度计算、LQR、UDP回调、电压/触摸等），便于兼容迁移。
#ifndef I2C_MODULE_H
#define I2C_MODULE_H

// 引入Arduino基础类型与API。
// 作用：提供基本类型（如uint8_t、uint16_t）以及Arduino生命周期函数定义。
// 为什么这样写：头文件中仅包含必要依赖，避免过多耦合。
#include <Arduino.h>

// 引入异步UDP数据包类型声明。
// 作用：声明回调函数参数类型AsyncUDPPacket。
// 为什么这样写：onPacketCallBack需要使用该类型，放在头文件中暴露原型。
#include <AsyncUDP.h>

// 对外函数原型声明，便于其他源文件调用这些功能。
// 作用：提供姿态角计算、角度约束、LQR控制、电压检测、WiFi配置、触摸事件等接口。
// 为什么这样写：将接口与实现分离，符合“声明-实现”解耦原则。

// WiFi打印辅助函数：将键值对格式化到发送缓冲区。
void wifi_print(char *s, double num);

// 加速度值转换为角度。
double acc2rotation(double x, double y);

// 将角度约束在[-60,60]范围的辅助函数。
float constrainAngle(float x);

// LQR控制器：根据摆角、角速度、电机速度计算输出电压。
float controllerLQR(float p_angle, float p_vel, float m_vel);

// UDP数据包回调处理：解析命令、提交EEPROM等。
void onPacketCallBack(AsyncUDPPacket packet);

// 电池电压检测逻辑：低电压保护与指示。
void voltage_detection();

// 读取指定引脚的输入电压（经分压换算）。
double return_voltage_value(int pin_no);

// WiFi自动配置（AP模式 + OTA）。
void AutoWifiConfig();

// 触摸事件处理：单击。
void single_event(int touchID);

// 触摸事件处理：长按。
void long_event(int touchID);

#endif // I2C_MODULE_H