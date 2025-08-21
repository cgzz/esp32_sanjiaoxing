// 电机FOC配置与控制：初始化BLDC驱动与传感器、加载/保存零电角、运行loopFOC，提供移动与控制模式切换接口，导出全局BLDCMotor对象。
#include "motor_foc.h"
#include "encoder_generic.h"
#include "my_config.h"

static BLDCDriver3PWM driver(32, 33, 25);
static GenericSensor sensor(encoderGenericRead, encoderGenericInit);
BLDCMotor motor = BLDCMotor(7);

void motorFocSetup()
{
  sensor.init();
  motor.linkSensor(&sensor);

  driver.voltage_power_supply = 12;
  driver.init();
  motor.linkDriver(&driver);

  motor.foc_modulation = SpaceVectorPWM;
  motor.controller = torque;

  motor.PID_velocity.P = v_p_1;
  motor.PID_velocity.I = v_i_1;

  motor.voltage_limit = 12;
  motor.LPF_velocity.Tf = 0.01f;
  motor.velocity_limit = 40;

  motor.useMonitoring(Serial);
  motor.init();
  motor.initFOC();
}
