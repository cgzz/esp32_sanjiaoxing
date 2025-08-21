

#include <Arduino.h>
#include <SimpleFOC.h>

extern MotionControlType mode; // torque or velocity
extern float motion_target;    // voltage if torque, velocity if velocity
extern bool isStable;          // 当前是否稳态，用于外部切换PI参数
void blance_compute();
void swingup_compute();