// 参数中心接口：集中声明LQR、速度环PI、目标/阈值与运行标志的getter/setter，便于全局统一管理。
#ifndef PARAMS_MODULE_H
#define PARAMS_MODULE_H

#include <Arduino.h>

extern float LQR_K3_1;
extern float LQR_K3_2;
extern float LQR_K3_3;
extern float LQR_K4_1;
extern float LQR_K4_2;
extern float LQR_K4_3;
extern float v_p_1;
extern float v_i_1;
extern float v_p_2;
extern float v_i_2;
extern float target_velocity;
extern float target_angle;
extern float target_voltage;
extern float swing_up_voltage;
extern float swing_up_angle;
extern int test_flag;
extern bool motor_enable_flag;

extern float err_angle;
extern bool blance_swingup;
#endif // PARAMS_MODULE_H