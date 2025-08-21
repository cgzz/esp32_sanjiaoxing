// 通用编码器接口：声明GPIO时序读角度与初始化管脚函数，供FOC传感器适配调用。
#ifndef ENCODER_GENERIC_MODULE_H
#define ENCODER_GENERIC_MODULE_H

#include <Arduino.h>

void encoderGenericInit();
float encoderGenericRead();

#endif // ENCODER_GENERIC_MODULE_H 