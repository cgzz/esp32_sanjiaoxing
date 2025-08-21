// 程序入口：初始化存储、UI/触摸与RGB、IMU与传感器融合、FOC电机、命令系统与WiFi/OTA；主循环执行FOC、UI触摸、周期性电池检测、IMU更新与平衡控制，并根据模式驱动电机与遥测。
#include <Arduino.h>
#include <SimpleFOC.h>

#include "my_config.h"
#include "motor_foc.h"
#include "mpu6050_hal.h"
#include "sensor_fusion.h"
#include "balance_control.h"
#include "my_web.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ESP32 IOs / constants
#define ACTIVE_PIN 4
#define BAT_VOLTAGE_SENSE_PIN 34
static const double R1_VOLTAGE = 62000;
static const double R2_VOLTAGE = 10000;
static const double MIN_VOLTAGE = 9;

static uint32_t lastVoltageMs = 0;
int16_t ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

static TaskHandle_t data_send_TaskHandle = nullptr; // 遥测 FreeRTOS 任务
static TaskHandle_t control_TaskHandle = nullptr;
float zero_angle = 0;
void data_send_Task(void *)
{
  for (;;)
  {
    uint32_t dt_ms = my_web_data_update();
    send_msg[0] = ax / 131; // 摆角
    send_msg[1] = ay / 131; // 侧倾角
    send_msg[2] = az / 131; // 纵向角

    send_msg[3] = 0;         // 角速度X
    send_msg[4] = gyroZrate; // 角速度Y
    send_msg[5] = 0;         // 角速度Z

    send_msg[6] = err_angle;
    send_msg[7] = kalAngleZ - zero_angle;
    send_msg[8] = 0;

    send_msg[9] = motion_target;
    send_msg[10] = 0;
    send_msg[11] = 0;
    send_fall = blance_swingup;
    vTaskDelay(pdMS_TO_TICKS(dt_ms));
  }
}

void robot_control_Task(void *)
{
  for (;;)
  {
    // 读取IMU
    if (MPU6050HAL::readRaw(ax, ay, az, gx, gy, gz))
      update(ax, ay, az, gx, gy, gz);
    err_angle = constrainAngle(fmod(kalAngleZ - zero_angle, 120.0f) - target_angle);
    gyroZrate = gz / 131.0f; // 注意：这里gz来自上面的readRaw
    // =============================================
    if (testmode_enabled)
    {
      motion_target = testmode_value;
      motor.controller = (MotionControlType)(testmode_motor_mode);
    }
    else
    {
      if (abs(err_angle) < swing_up_angle) // 如果角度小于摆动角度阈值，执行平衡控制
        blance_compute();
      else
        swingup_compute();
    }
    // =============================================
    if (!robot_run)
      motion_target = 0;
    motor.target = motion_target;
    motor.loopFOC();
    motor.move();
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(ACTIVE_PIN, OUTPUT);
  digitalWrite(ACTIVE_PIN, LOW);

  // IMU init
  MPU6050HAL::begin(19, 18, 400000);
  if (!MPU6050HAL::configure())
  {
    Serial.println("Error reading sensor");
    while (1)
    {
    }
  }
  MPU6050HAL::readRaw(ax, ay, az, gx, gy, gz);
  float pitch = acc2rotation((float)ax, (float)ay);
  initWithPitch(pitch);
  Serial.println("kalman mpu6050 init");
  // 新增初始化
  if (MPU6050HAL::readRaw(ax, ay, az, gx, gy, gz))
    update(ax, ay, az, gx, gy, gz);
  zero_angle = kalAngleZ;

  // FOC 初始化
  motorFocSetup();
  my_wifi_init();
  my_web_asyn_init();
  digitalWrite(ACTIVE_PIN, HIGH);

  xTaskCreatePinnedToCore(robot_control_Task, "ctrl_2ms", 8192, nullptr, 20, &control_TaskHandle, 0);
  xTaskCreatePinnedToCore(data_send_Task, "telem", 8192, nullptr, 1, &data_send_TaskHandle, 1);
}

void loop()
{
}
