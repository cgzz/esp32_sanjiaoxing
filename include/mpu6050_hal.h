// MPU6050 HAL接口：声明I2C初始化、设备配置与原始加速度/陀螺仪读取函数。
#ifndef MPU6050_HAL_MODULE_H
#define MPU6050_HAL_MODULE_H

#include <Arduino.h>

namespace MPU6050HAL {
  bool begin(int sdaPin, int sclPin, uint32_t clockHz = 400000);
  bool configure();
  bool readRaw(int16_t& ax, int16_t& ay, int16_t& az,
               int16_t& gx, int16_t& gy, int16_t& gz);
}

#endif // MPU6050_HAL_MODULE_H 