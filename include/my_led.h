#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// 硬件参数
constexpr uint8_t LED_PIN = 16;
constexpr uint16_t NUM_LEDS = 21;
constexpr uint8_t LED_TYPE = NEO_GRB + NEO_KHZ800;

// 模式定义
enum LedMode : uint8_t
{
    MODE_CHASE = 0,
    MODE_COMET,
    MODE_BOUNCE,
    MODE_THEATER,
    MODE_RAINBOW,
    MODE_BREATH,
    MODE_COUNT
};

struct LedConfig
{
    LedMode mode = MODE_CHASE;
    int8_t dir = 1;
    uint16_t interval = 40;
    uint8_t brightness = 80;
    uint32_t color = 0x1EC8FF; // 0xRRGGBB
    uint8_t tailDecay = 48;
    uint8_t theaterStep = 3;
    uint8_t breathSpeed = 8;
    uint8_t rainbowSpeed = 4;
    bool powerOn = false;
};

void leds_init();
void leds_update(); // 每帧调用（非阻塞）
void leds_apply(const LedConfig &c);
LedConfig leds_get();

void leds_power(bool on);
