// MPU6050 硬件抽象层：I2C寄存器读写、设备配置、WHO_AM_I检测与原始六轴数据读取。
#include "mpu6050_hal.h"
#include <Wire.h>

namespace
{
  constexpr uint8_t MPU_ADDR = 0x68; // AD0 = GND

  bool i2cWrite(uint8_t reg, uint8_t data, bool sendStop = true)
  {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(data);
    return Wire.endTransmission(sendStop) != 0;
  }
  bool i2cWrite(uint8_t reg, const uint8_t *data, size_t len, bool sendStop = true)
  {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(data, len);
    return Wire.endTransmission(sendStop) != 0;
  }
  bool i2cRead(uint8_t reg, uint8_t *data, size_t len)
  {
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0)
      return true;
    Wire.requestFrom((int)MPU_ADDR, (int)len);
    size_t idx = 0;
    while (Wire.available() && idx < len)
    {
      data[idx++] = Wire.read();
    }
    return idx != len;
  }
}

namespace MPU6050HAL
{
  bool begin(int sdaPin, int sclPin, uint32_t clockHz)
  {
    Wire.begin(sdaPin, sclPin, clockHz);
    delay(10);
    return true;
  }

  bool configure()
  {
    uint8_t whoami = 0;
    if (i2cRead(0x75, &whoami, 1))
      return false;
    if (whoami != 0x68 && whoami != 0x70)
      return false;

    uint8_t cfg[4];
    cfg[0] = 7;    // 0x19: sample rate to 1kHz
    cfg[1] = 0x00; // 0x1A: DLPF config
    cfg[2] = 0x00; // 0x1B: Gyro FS ±250 deg/s
    cfg[3] = 0x00; // 0x1C: Accel FS ±2g
    if (i2cWrite(0x19, cfg, 4, true))
      return false;
    if (i2cWrite(0x6B, 0x01, true))
      return false; // PLL with X gyro, wake up
    delay(100);
    return true;
  }

  bool readRaw(int16_t &ax, int16_t &ay, int16_t &az,
               int16_t &gx, int16_t &gy, int16_t &gz)
  {
    uint8_t buf[14];
    if (i2cRead(0x3B, buf, 14))
      return false;
    ax = (int16_t)((buf[0] << 8) | buf[1]);
    ay = (int16_t)((buf[2] << 8) | buf[3]);
    az = (int16_t)((buf[4] << 8) | buf[5]);
    // int16_t temp = (int16_t)((buf[6] << 8) | buf[7]);
    gx = (int16_t)((buf[8] << 8) | buf[9]);
    gy = (int16_t)((buf[10] << 8) | buf[11]);
    gz = (int16_t)((buf[12] << 8) | buf[13]);
    return true;
  }
}