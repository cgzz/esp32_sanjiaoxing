// 通用编码器SPI读取：以GPIO手动时序读取16位角度数据并转换为弧度，初始化管脚模式。
#include "encoder_generic.h"

float encoderGenericRead() {
  digitalWrite(22, 0);
  digitalWrite(5, 0);
  uint16_t ag = 0;
  for (int i = 0; i < 16; i++) {
    digitalWrite(5, 1);
    delayMicroseconds(1);
    ag = (uint16_t)(ag << 1);
    ag = (uint16_t)(ag + (uint16_t)digitalRead(23));
    digitalWrite(5, 0);
    delayMicroseconds(1);
  }
  digitalWrite(22, 1);
  ag = (uint16_t)(ag >> 2);
  digitalWrite(5, 1);
  float rad = (float)ag * 2.0f * PI / 16384.0f;
  if (rad < 0) rad += 2.0f * PI;
  return rad;
}

void encoderGenericInit() {
  pinMode(5, OUTPUT);
  pinMode(22, OUTPUT);
  pinMode(23, INPUT);
} 