// 参数中心：集中保存与提供LQR、速度环PI、目标与阈值、运行状态等可调/运行参数的getter/setter。
#include "my_config.h"

// 默认值与运行时状态
float LQR_K3_1 = 12.0f;
float LQR_K3_2 = 1.9f;
float LQR_K3_3 = 1.8f;

float LQR_K4_1 = 4.4f;
float LQR_K4_2 = 1.5f;
float LQR_K4_3 = 1.42f;
float v_p_1 = 0.25f;
float v_i_1 = 15.0f;
float v_p_2 = 0.10f;
float v_i_2 = 10.0f;
float target_velocity = 0.0f;
float target_angle = 60.0f;
float target_voltage = 0.0f;
float swing_up_voltage = 1.0f;
float swing_up_angle = 18.0f;
int test_flag = 0;
bool motor_enable_flag = false;

float err_angle = 0;
float gyroZrate = 0;
bool blance_swingup = false;
