// 程序入口：初始化存储、UI/触摸与RGB、IMU与传感器融合、FOC电机、命令系统与WiFi/OTA；主循环执行FOC、UI触摸、周期性电池检测、IMU更新与平衡控制，并根据模式驱动电机与遥测。
#include <Arduino.h>
#include <SimpleFOC.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sensor_fusion.h"

#include "my_io.h"
#include "my_foc.h"
#include "my_mpu6050.h"
#include "my_control.h"
#include "my_web.h"

static TaskHandle_t data_send_TaskHandle = nullptr; // 遥测 FreeRTOS 任务
static TaskHandle_t control_TaskHandle = nullptr;   // 控制 FreeRTOS 任务
static TaskHandle_t led_TaskHandle = nullptr;       // LED FreeRTOS 任务
// 三个图表的名称和三个通道的名称
ChartConfig chart_config[3] = {
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
};
// 12个滑块组的名称
SliderGroup slider_group[4] = {
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
    {"", {"", "", ""}},
};
// 网页参数推送
void data_send_Task(void *)
{
  for (;;)
  {
    uint32_t dt_ms = my_web_data_update();
    // 这个地方可以放自己想看的参数
    // 姿态角
    send_msg[0] = now_angleX; // 摆角
    send_msg[1] = now_angleY; // 侧倾角
    send_msg[2] = now_angleZ; // 纵向角
    // 图1
    send_msg[3] = 0;         // 角速度X
    send_msg[4] = now_gyroZ; // 角速度Y
    send_msg[5] = 0;         // 角速度Z
    // 图2
    send_msg[6] = err_angle;
    send_msg[7] = kalAngleZ - angleZ0;
    send_msg[8] = 0;
    // 图3
    send_msg[9] = motion_target;
    send_msg[10] = 0;
    send_msg[11] = 0;
    // 状态
    send_fall = blance_swingup;
    vTaskDelay(pdMS_TO_TICKS(dt_ms));
  }
}
// 运动控制
void robot_control_Task(void *)
{
  for (;;)
  {
    // 读取IMU
    mpu6050_update();
    // 卡尔曼滤波更新角度
    kalman_update();
    // 运动控制更新
    move_update();
    // 电机运动
    motor_update();
  }
}
// LED
void led_Task(void *)
{
  for (;;)
  {
    // 控制LED状态
  }
}

void setup()
{
  pinMode(ACTIVE_PIN, OUTPUT);
  digitalWrite(ACTIVE_PIN, LOW);
  Serial.begin(115200);
  // wifi初始化
  my_wifi_init();
  // web初始化
  my_web_asyn_init();
  // web ui文字传入
  my_web_ui_init(slider_group, chart_config);
  // io初始化
  my_io_init();
  // 6050 初始化
  mpu6050_init();
  initWithPitch(acc2rotation(angleX0, angleY0));
  kalman_update();
  angleZ0 = kalAngleZ;
  // FOC 初始化
  motor_init();

  digitalWrite(ACTIVE_PIN, HIGH);
  // 启动网页和控制任务
  xTaskCreatePinnedToCore(robot_control_Task, "control", 8192, nullptr, 20, &control_TaskHandle, 0);
  xTaskCreatePinnedToCore(data_send_Task, "websend", 8192, nullptr, 1, &data_send_TaskHandle, 1);
  xTaskCreatePinnedToCore(data_send_Task, "led", 8192, nullptr, 1, &led_TaskHandle, 1);
}

void loop()
{
}
