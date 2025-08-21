#include "my_led_config.h"

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, LED_TYPE);
LedConfig led_config;
uint32_t tPrev = 0;
int32_t idx = 0;
int8_t dir = 1;
uint16_t phase = 0;

void leds_init()
{
    strip.begin();
    strip.setBrightness(led_config.brightness);
    strip.clear();
    strip.show();
}

void leds_power(bool on)
{
    led_config.powerOn = on;
    if (!on)
    {
        strip.clear();
        strip.show();
    }
}

void leds_apply()
{
    strip.setBrightness(led_config.brightness);
    dir = (led_config.dir >= 0) ? 1 : -1;
    idx = 0;
    phase = 0;
    if (!led_config.powerOn)
    {
        strip.clear();
        strip.show();
    }
}

void leds_update()
{
    if (!led_config.powerOn)
        return;
    const uint32_t now = millis();
    if (now - tPrev < led_config.interval)
        return;
    tPrev = now;

    switch (led_config.mode)
    {
    case 0: // 追逐模式
        led_mode_chase();
    case 1: // 彗星
        led_mode_comet();
    case 2: // 弹跳
        led_mode_bounce();
    case 3: // 剧院
        led_mode_theater();
    case 4: // 彩虹
        led_mode_rainbow();
    case 5: // 呼吸
        led_mode_breath();
    case 6: // 自定义模式1
        led_mode_custom1();
    case 7: // 自定义模式2
        led_mode_custom2();
    case 8: // 自定义模式3
        led_mode_custom3();
    }
}
