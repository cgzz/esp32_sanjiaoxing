#include "my_led.h"

static Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, LED_TYPE);
static LedConfig cfg;
static uint32_t tPrev = 0;
static int32_t idx = 0;
static int8_t dir = 1;
static uint16_t phase = 0;

static uint16_t wrap(int32_t x)
{
    int32_t n = NUM_LEDS;
    int32_t m = x % n;
    return (m < 0) ? (m + n) : m;
}
static uint32_t wheel(uint8_t pos)
{
    pos = 255 - pos;
    if (pos < 85)
        return strip.Color(255 - pos * 3, 0, pos * 3);
    if (pos < 170)
    {
        pos -= 85;
        return strip.Color(0, pos * 3, 255 - pos * 3);
    }
    pos -= 170;
    return strip.Color(pos * 3, 255 - pos * 3, 0);
}
static void fadeAll(uint8_t decay)
{
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
        uint32_t c = strip.getPixelColor(i);
        uint8_t r = (c >> 16) & 0xFF, g = (c >> 8) & 0xFF, b = c & 0xFF;
        r = (r > decay) ? (r - decay) : 0;
        g = (g > decay) ? (g - decay) : 0;
        b = (b > decay) ? (b - decay) : 0;
        strip.setPixelColor(i, r, g, b);
    }
}

void leds_init()
{
    strip.begin();
    strip.setBrightness(cfg.brightness);
    strip.clear();
    strip.show();
}

void leds_power(bool on)
{
    cfg.powerOn = on;
    if (!on)
    {
        strip.clear();
        strip.show();
    }
}

void leds_apply(const LedConfig &c)
{
    cfg = c;
    strip.setBrightness(cfg.brightness);
    dir = (cfg.dir >= 0) ? 1 : -1;
    idx = 0;
    phase = 0;
    if (!cfg.powerOn)
    {
        strip.clear();
        strip.show();
    }
}

LedConfig leds_get() { return cfg; }

void leds_update()
{
    if (!cfg.powerOn)
        return;
    const uint32_t now = millis();
    if (now - tPrev < cfg.interval)
        return;
    tPrev = now;

    switch (cfg.mode)
    {
    case MODE_CHASE:
        strip.clear();
        strip.setPixelColor(wrap(idx), cfg.color);
        strip.show();
        idx = wrap(idx + dir);
        break;

    case MODE_COMET:
        fadeAll(cfg.tailDecay);
        strip.setPixelColor(wrap(idx), cfg.color);
        strip.show();
        idx = wrap(idx + dir);
        break;

    case MODE_BOUNCE:
    {
        strip.clear();
        strip.setPixelColor(wrap(idx), cfg.color);
        strip.show();
        idx += dir;
        if (idx >= (int)NUM_LEDS - 1)
        {
            idx = NUM_LEDS - 1;
            dir = -1;
        }
        else if (idx <= 0)
        {
            idx = 0;
            dir = 1;
        }
    }
    break;

    case MODE_THEATER:
    {
        strip.clear();
        for (uint16_t i = 0; i < NUM_LEDS; i++)
            if (((i + idx) % cfg.theaterStep) == 0)
                strip.setPixelColor(i, cfg.color);
        strip.show();
        idx = wrap(idx + dir);
    }
    break;

    case MODE_RAINBOW:
    {
        for (uint16_t i = 0; i < NUM_LEDS; i++)
        {
            uint8_t hue = (uint8_t)((i * 256 / NUM_LEDS + phase) & 0xFF);
            strip.setPixelColor(i, wheel(hue));
        }
        strip.show();
        phase = (phase + cfg.rainbowSpeed) & 0xFFFF;
    }
    break;

    case MODE_BREATH:
    {
        phase = (phase + cfg.breathSpeed) & 0x03FF; // 0..1023
        float t = (float)phase / 1024.0f;
        float s = 0.5f - 0.5f * cosf(2.0f * PI * t);
        uint8_t r = ((cfg.color >> 16) & 0xFF) * s;
        uint8_t g = ((cfg.color >> 8) & 0xFF) * s;
        uint8_t b = ((cfg.color) & 0xFF) * s;
        for (uint16_t i = 0; i < NUM_LEDS; i++)
            strip.setPixelColor(i, r, g, b);
        strip.show();
    }
    break;

    default:
        break;
    }
}
