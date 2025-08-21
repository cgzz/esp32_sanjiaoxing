// 传感器融合接口：声明姿态初始化、加速度转角、角度约束、原始六轴更新与角度/角速度输出API。
#ifndef SENSOR_FUSION_MODULE_H
#define SENSOR_FUSION_MODULE_H

#include <Arduino.h>

void initWithPitch(float pitchDeg);
float acc2rotation(float x, float y);
float constrainAngle(float x);
// 输入原始IMU，内部维护dt（通过micros）
void update(int16_t ax, int16_t ay, int16_t az, int16_t gx, int16_t gy, int16_t gz);

extern float kalAngleZ;
#endif // SENSOR_FUSION_MODULE_H