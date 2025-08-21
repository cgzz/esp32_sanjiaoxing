// 平衡控制接口：定义控制输出结构与核心compute函数原型，供主循环与FOC控制调用。
#ifndef BALANCE_CONTROL_MODULE_H
#define BALANCE_CONTROL_MODULE_H

#include <Arduino.h>
#include <SimpleFOC.h>

extern MotionControlType mode; // torque or velocity
extern float motion_target;    // voltage if torque, velocity if velocity
extern bool isStable;          // 当前是否稳态，用于外部切换PI参数
void blance_compute();
void swingup_compute();
#endif // BALANCE_CONTROL_MODULE_H