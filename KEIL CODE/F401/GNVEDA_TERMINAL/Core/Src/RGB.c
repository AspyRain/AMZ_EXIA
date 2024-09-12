#include "RGB.h"
#include "main.h"
#include <rtthread.h>
#include <stdlib.h>

// 定义 RGB_LED 结构体数组
RGB_LED* leds[Led_Num];
int led_index = 0;

/**
 * @brief 初始化 RGB_LED 结构体
 * @param rgb_led: RGB_LED 结构体指针
 * @param led_num: 灯珠数量
 * @param htim: TIM_HandleTypeDef 指针
 * @param TIM_CHANNEL: TIM 通道
 */
void RGB_LED_Init(RGB_LED *rgb_led, int led_num, TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL)
{
    if (rgb_led == NULL)
    {
        //rt_kprintf("Pointer to RGB_LED structure is NULL.\n");
        return; // 返回
    }

    // 设置 RGB_LED 结构体成员
    rgb_led->led_num = led_num;
    rgb_led->htim = htim;
    rgb_led->TIM_CHANNEL = TIM_CHANNEL;

    // 分配 RGB_buffer
    rgb_led->RGB_buffer = (uint16_t *)malloc((Reste_Data + Led_Data_Len * led_num) * sizeof(uint16_t));

    if (rgb_led->RGB_buffer == NULL)
    {
        //rt_kprintf("Failed to allocate memory for RGB_buffer.\n");
        return; // 返回
    }

    // 初始化 RGB_buffer 为全黑色
    for (int i = 0; i < led_num; i++)
    {
        WS2812_Set_Color(rgb_led, 0, i);
    }

    // 将 RGB_LED 结构体添加到 leds 数组中
    leds[led_index++] = rgb_led;
    rt_kprintf("初始化WS2812完成:\n灯珠数:%d\n",led_num);
}

/**
 * @brief 设置单个 LED 的颜色
 * @param rgb_led: RGB_LED 结构体指针
 * @param Color: 颜色值 (0xRRGGBB)
 * @param num: LED 的索引
 */
void WS2812_Set_Color(RGB_LED *rgb_led, uint32_t Color, uint16_t num)
{
    // 获取 LED 在 RGB_buffer 中的起始位置
    uint16_t *p = (rgb_led->RGB_buffer + Reste_Data) + (num * Led_Data_Len);

    // 设置 LED 的红色部分
    for (uint8_t i = 0; i < 8; ++i)
        p[i + 8] = (((Color << i) & 0X800000) ? Hight_Data : Low_Data);

    // 设置 LED 的绿色部分
    for (uint8_t i = 8; i < 16; ++i)
        p[i - 8] = (((Color << i) & 0X800000) ? Hight_Data : Low_Data);

    // 设置 LED 的蓝色部分
    for (uint8_t i = 16; i < 24; ++i)
        p[i] = (((Color << i) & 0X800000) ? Hight_Data : Low_Data);
}

/**
 * @brief 设置单个 LED 的 RGB 颜色
 * @param rgb_led: RGB_LED 结构体指针
 * @param red: 红色分量
 * @param green: 绿色分量
 * @param blue: 蓝色分量
 * @param num: LED 的索引
 */
void WS2812_Set_RGB(RGB_LED *rgb_led, uint8_t red, uint8_t green, uint8_t blue, uint16_t num)
{
    // 构造颜色值
    uint32_t Color = (green << 16 | red << 8 | blue);

    // 获取 LED 在 RGB_buffer 中的起始位置
    uint16_t *p = (rgb_led->RGB_buffer + Reste_Data) + (num * Led_Data_Len);

    // 设置 LED 的颜色
    for (uint8_t i = 0; i < 24; ++i)
        p[i] = (((Color << i) & 0X800000) ? Hight_Data : Low_Data);
}

/**
 * @brief 更新显示
 * @param rgb_led: RGB_LED 结构体指针
 */
void WS2812_Show(RGB_LED *rgb_led)
{
    HAL_TIM_PWM_Start_DMA(rgb_led->htim, rgb_led->TIM_CHANNEL, (uint32_t *)rgb_led->RGB_buffer, (176)); // 启动 DMA
}

/**
 * @brief DMA 完成回调函数
 * @param htim: TIM_HandleTypeDef 指针
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 0; i < Led_Num; i++)
    {
        if (htim == leds[i]->htim)
        {
            HAL_TIM_PWM_Stop_DMA(leds[i]->htim, leds[i]->TIM_CHANNEL);
            break; // 停止 DMA
        }
    }
}

// 颜色分量提取函数
uint8_t getRed(uint32_t color) {
    return (color >> 16) & 0xFF;
}

uint8_t getGreen(uint32_t color) {
    return (color >> 8) & 0xFF;
}

uint8_t getBlue(uint32_t color) {
    return color & 0xFF;
}

/**
 * @brief 执行颜色渐变
 * @param rgb_led: RGB_LED 结构体指针
 * @param beginColor: 开始颜色
 * @param endColor: 结束颜色
 * @param duration: 渐变持续时间 (毫秒)
 * @param indexes: LED 索引数组
 * @param num: LED 数量
 */
void colorGradient(RGB_LED *rgb_led, uint32_t beginColor, uint32_t endColor, uint16_t duration, int indexes[], int num)
{
    // 解析起始和结束颜色
    uint8_t r_start = getRed(beginColor);
    uint8_t g_start = getGreen(beginColor);
    uint8_t b_start = getBlue(beginColor);
    uint8_t r_end = getRed(endColor);
    uint8_t g_end = getGreen(endColor);
    uint8_t b_end = getBlue(endColor);

    // 计算每种颜色成分的变化量
    int dr = r_end - r_start;
    int dg = g_end - g_start;
    int db = b_end - b_start;

    // 总变化步数
    uint16_t steps = (duration / 10) + 1; // 每10毫秒一步
    uint16_t delay_time = duration / steps; // 每步的延迟时间

    // 开始渐变
    for (uint16_t step = 0; step <= steps; step++) // 添加等号以确保最后一步达到目标颜色
    {
        // 更新每种颜色成分
        float t = (float)step / steps; // 当前步数相对于总步数的比例
        uint8_t r = r_start + (dr * t);
        uint8_t g = g_start + (dg * t);
        uint8_t b = b_start + (db * t);

        // 设置 LED 颜色
        for (uint16_t i = 0; i < num; i++)
        {
            WS2812_Set_RGB(rgb_led, r, g, b, indexes[i]);
        }

        // 更新显示
        WS2812_Show(rgb_led);

        // 延迟
        if (step < steps) // 只在非最后一步时延迟
        {
            rt_thread_mdelay(delay_time);
        }
    }
}