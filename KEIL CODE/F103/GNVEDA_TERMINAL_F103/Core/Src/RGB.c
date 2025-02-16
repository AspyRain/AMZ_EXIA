#include "RGB.h"
#include "main.h"
#include <rtthread.h>
#include <stdlib.h>

// 定义 RGB_LED 结构体数组
RGB_LED *leds[Leds_Num];
int led_index = 0;

/**
 * @brief 初始化 RGB_LED 结构体
 * @param rgb_led: RGB_LED 结构体指针
 * @param led_num: 灯珠数量
 * @param htim: TIM_HandleTypeDef 指针
 * @param TIM_CHANNEL: TIM 通道
 */
// 预先定义一个足够大的数组
static uint16_t static_RGB_buffer[5120]; // 假设最多支持5120字节的LED数据

void RGB_LED_Init(RGB_LED *rgb_led, int led_num, TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL)
{
    if (rgb_led == NULL)
    {
        rt_kprintf("Pointer to RGB_LED structure is NULL.\n");
        return; // 返回
    }

    // 设置 RGB_LED 结构体成员
    rgb_led->led_num = led_num;
    rgb_led->htim = htim;
    rgb_led->TIM_CHANNEL = TIM_CHANNEL;

    rt_kprintf("step 2\n");

    // 计算需要的内存大小
    size_t buffer_size = (Reset_Data + Led_Data_Len * led_num) * sizeof(uint16_t);
    rt_kprintf("预计使用堆: %lu 字节\n", buffer_size);

    if (buffer_size > sizeof(static_RGB_buffer))
    {
        rt_kprintf("内存不足，请求大小：%lu，静态缓冲区大小：%lu\n", buffer_size, sizeof(static_RGB_buffer));
        return;
    }

    // 使用静态内存
    rgb_led->RGB_buffer = static_RGB_buffer;

    rt_kprintf("step 3\n");

    // 初始化 RGB_buffer 为全黑色
    for (int i = 0; i < led_num; i++)
    {
        WS2812_Set_Color(rgb_led, 0, i);
    }

    rt_kprintf("step 4\n");

    // 将 RGB_LED 结构体添加到 leds 数组中
    leds[led_index++] = rgb_led;
    rt_kprintf("初始化WS2812完成:\n灯珠数:%d\n", led_num);
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
    uint16_t *p = (rgb_led->RGB_buffer + Reset_Data) + (num * Led_Data_Len);
    //切换为GRB
    uint8_t r = (Color >> 16) & 0xFF;
    uint8_t g = (Color >> 8) & 0xFF;
    uint8_t b = Color & 0xFF;

    uint32_t grb = (g << 16) | (r << 8) | b;
    for (uint8_t i = 0; i < 24; ++i)
        p[i] = (((grb << i) & 0X800000) ? Hight_Data : Low_Data);
}

/**
 * @brief 设置单个 LED 的 RGB 颜色
 * @param rgb_led: RGB_LED 结构体指针
 * @param red: 红色分量
 * @param green: 绿色分量
 * @param blue: 蓝色分量
 * @param num: LED 的索引
 */
void WS2812_Set_RGB(RGB_LED *rgb_led, uint8_t red, uint8_t green, uint8_t blue, uint16_t index)
{
    // 构造颜色值
    uint32_t Color = (green << 16 | red << 8 | blue);

    // 获取 LED 在 RGB_buffer 中的起始位置
    uint16_t *p = (rgb_led->RGB_buffer + Reset_Data) + (index * Led_Data_Len);

    // 设置 LED 的颜色
    for (uint8_t i = 0; i < 24; ++i)
        p[i] = (((Color << i) & 0X800000) ? Hight_Data : Low_Data);
}

/**
 * @brief 更新所有LED的颜色显示
 * @param rgb_led: RGB_LED 结构体指针
 */
void WS2812_Show(RGB_LED *rgb_led)
{
    if (rgb_led == NULL)
    {
        rt_kprintf("RGB_LED structure pointer is NULL.\n");
        return; // 如果指针为空，直接返回
    }

    // 计算DMA传输的数据长度
    uint32_t data_length = Reset_Data + Led_Data_Len * rgb_led->led_num;
    // 启动DMA传输
    HAL_TIM_PWM_Start_DMA(rgb_led->htim, rgb_led->TIM_CHANNEL, (uint32_t *)rgb_led->RGB_buffer, data_length);
}

/**
 * @brief DMA 完成回调函数
 * @param htim: TIM_HandleTypeDef 指针
 */
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
    for (int i = 0; i < Leds_Num; i++)
    {
        if (htim == leds[i]->htim)
        {
            HAL_TIM_PWM_Stop_DMA(leds[i]->htim, leds[i]->TIM_CHANNEL);
            break; // 停止 DMA
        }
    }
}

// 颜色分量提取函数
uint8_t getRed(uint32_t color)
{
    return (color >> 16) & 0xFF;
}

uint8_t getGreen(uint32_t color)
{
    return (color >> 8) & 0xFF;
}

uint8_t getBlue(uint32_t color)
{
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
    uint16_t steps = (duration / 10) + 1;   // 每10毫秒一步
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

void resetColor(RGB_LED *rgb_led, int indexes[], int num)
{
    if (num == 0)
    {
        for (uint16_t i = 0; i < rgb_led->led_num; i++)
        {
            WS2812_Set_RGB(rgb_led, 0, 0, 0, i);
        }
    }
    else
    {
        for (uint16_t i = 0; i < num; i++)
        {
            WS2812_Set_RGB(rgb_led, 0, 0, 0, indexes[i]);
        }
    }
}