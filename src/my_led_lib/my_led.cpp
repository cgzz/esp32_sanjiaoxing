#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
// 直接宏定义引脚
#define LED_PIN 16
// 21 个 LED

// GPIO4
const uint16_t NUM_LEDS = 21;

// 默认颜色（R,G,B）
uint8_t colorR = 0;
uint8_t colorG = 150;
uint8_t colorB = 0;

// 每步间隔（毫秒），控制跑马灯速度；可以通过串口修改
unsigned long stepDelay = 150; // 默认 150 ms

// 创建对象strip
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// 运行状态变量
int currentIndex = 0;         // 现在亮的灯编号（0..NUM_LEDS-1）
unsigned long lastStepMs = 0; // 记录上一次切换的时间（毫秒）

void led_init()
{
    Serial.begin(115200);
    strip.begin();
    strip.setBrightness(80);
    // 初始全部关灯
    for (uint16_t i = 0; i < NUM_LEDS; ++i)
        strip.setPixelColor(i, 0);
    strip.show();
    // 先点亮第一个灯，让小朋友看到有灯
    uint32_t c = strip.Color(colorR, colorG, colorB);
    strip.setPixelColor(0, c);
    strip.show();
    // 初始化时间
    lastStepMs = millis();
    currentIndex = 0;
}

void led_update()
{
    unsigned long now = millis(); // 记录到每个灯亮的时候的时间
    // 非阻塞：到达时间再移动到下一个灯
    if (now - lastStepMs >= stepDelay)
    {
        lastStepMs = now;
        // 熄灭当前灯
        strip.setPixelColor(currentIndex, 0);
        // 移动到下一个编号
        currentIndex++;
        if (currentIndex >= (int)NUM_LEDS)
            currentIndex = 0; // 判断书否超出编号21
        // 点亮新的当前灯
        uint32_t c = strip.Color(colorR, colorG, colorB);
        strip.setPixelColor(currentIndex, c);
        // 发送数据到灯带
        strip.show();
    }

    // 串口控制（非阻塞读取）
    if (Serial.available())
    {
        char ch = Serial.read();
        if (ch == 'r')
        {
            colorR = 200;
            colorG = 0;
            colorB = 0;
            Serial.println("颜色: 红");
        }
        if (ch == 'g')
        {
            colorR = 0;
            colorG = 200;
            colorB = 0;
            Serial.println("颜色: 绿");
        }
        if (ch == 'b')
        {
            colorR = 0;
            colorG = 0;
            colorB = 200;
            Serial.println("颜色: 蓝");
        }
        if (ch == 'y')
        {
            colorR = 200;
            colorG = 200;
            colorB = 0;
            Serial.println("颜色: 黄");
        }
        if (ch == '1')
        {
            stepDelay = 400;
            Serial.println("速度: 慢");
        }
        if (ch == '2')
        {
            stepDelay = 200;
            Serial.println("速度: 中");
        }
        if (ch == '3')
        {
            stepDelay = 100;
            Serial.println("速度: 快");
        }
        if (ch == '4')
        {
            stepDelay = 50;
            Serial.println("速度: 非常快");
        }
        if (ch == 'c')
        {
            // 清屏：关掉所有灯
            for (uint16_t i = 0; i < NUM_LEDS; ++i)
                strip.setPixelColor(i, 0);
            strip.show();
            Serial.println("清屏：所有灯已关");
        }
    }
}