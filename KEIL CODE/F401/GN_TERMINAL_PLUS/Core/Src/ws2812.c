#include "ws2812.h"
#include <stdlib.h>
#include <rtthread.h>
#include <stdlib.h>
#include <string.h>
// ?? WS2812 ???
#define WS2812_CODE_1 (71u) // 0.8us
#define WS2812_CODE_0 (32u) // 0.4us

// ??????
static const uint32_t WS2812_RST[240] = {0}; 

// ??????
static uint32_t WS2812_En = 0;

void RGB_LED_Init(RGB_LED *rgb_led, int led_num, TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL)
{
    rgb_led->led_num = led_num;
    rgb_led->htim = htim;
    rgb_led->TIM_CHANNEL = TIM_CHANNEL;
    rgb_led->RGB_buffer = (uint16_t *)malloc(led_num * 3 * sizeof(uint16_t)); // ??LED??3?????RGB
}

void WS2812_Set_Color(RGB_LED *rgb_led, uint32_t Color, uint16_t index)
{
    uint8_t r = (Color & 0xFF0000) >> 16;
    uint8_t g = (Color & 0x00FF00) >> 8;
    uint8_t b = (Color & 0x0000FF);

    WS2812_Set_RGB(rgb_led, r, g, b, index);
}

void WS2812_Set_RGB(RGB_LED *rgb_led, uint8_t red, uint8_t green, uint8_t blue, uint16_t index)
{
    // ?RGB????WS2812???????
    uint32_t color = ((green << 16) | (red << 8) | blue);
    uint32_t *buffer = (uint32_t *)rgb_led->RGB_buffer + index * 24;

    // ????????PWM??
    for (int i = 0; i < 24; i++)
    {
        if (color & (1 << 23))
            *buffer++ = WS2812_CODE_1;
        else
            *buffer++ = WS2812_CODE_0;
        color <<= 1;
    }
}
/**
 * @brief ?uint32???????
 * @param Data:????
 * @param Ret:??????(PWM???)
 * @return
 * @author HZ12138
 * @date 2022-10-03 18:03:17
 */
void WS2812_uint32ToData(uint32_t Data, uint32_t *Ret)
{
    uint32_t zj = Data;
    uint8_t *p = (uint8_t *)&zj;
    uint8_t R = 0, G = 0, B = 0;
    B = *(p);     // B
    G = *(p + 1); // G
    R = *(p + 2); // R
    zj = (G << 16) | (R << 8) | B;
    for (int i = 0; i < 24; i++)
    {
        if (zj & (1 << 23))
            Ret[i] = WS2812_CODE_1;
        else
            Ret[i] = WS2812_CODE_0;
        zj <<= 1;
    }
    Ret[24] = 0;
}
void WS2812_Show(RGB_LED *rgb_led)
{
    // ?????RGB_buffer?????????DMA?????
    uint32_t *SendBuf0 = (uint32_t *)malloc(rgb_led->led_num * 25 * sizeof(uint32_t));
    uint32_t *SendBuf1 = (uint32_t *)malloc(rgb_led->led_num * 25 * sizeof(uint32_t));
    const uint32_t *Rst = (uint32_t *)malloc(240 * sizeof(uint32_t));

    if (!SendBuf0 || !SendBuf1 || !Rst)
    {
        // ??????
        free(SendBuf0);
        free(SendBuf1);
        free(Rst);
        return;
    }

    // ?????????
    memset(Rst, 0, 240 * sizeof(uint32_t));

    // ?????
    HAL_TIM_PWM_Start_DMA(rgb_led->htim, rgb_led->TIM_CHANNEL, (uint32_t *)Rst, 240);

    // ??RGB??
    for (int i = 0; i < rgb_led->led_num; i++)
    {
        uint32_t color = ((uint32_t)rgb_led->RGB_buffer[i * 3 + 1] << 16) |
                         ((uint32_t)rgb_led->RGB_buffer[i * 3 + 0] << 8) |
                         ((uint32_t)rgb_led->RGB_buffer[i * 3 + 2]);

        WS2812_uint32ToData(color, SendBuf0 + i * 25);
    }

    // ????????
    HAL_TIM_PWM_Start_DMA(rgb_led->htim, rgb_led->TIM_CHANNEL, SendBuf0, rgb_led->led_num * 25);

    // ??
    free((void *)Rst);
    free((void *)SendBuf0);
    free((void *)SendBuf1);
}

// ????????
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
 * @brief ??????
 * @param rgb_led: RGB_LED ?????
 * @param beginColor: ????
 * @param endColor: ????
 * @param duration: ?????? (??)
 * @param indexes: LED ????
 * @param num: LED ??
 */
void colorGradient(RGB_LED *rgb_led, uint32_t beginColor, uint32_t endColor, uint16_t duration, int indexes[], int num)
{
    // ?????????
    uint8_t r_start = getRed(beginColor);
    uint8_t g_start = getGreen(beginColor);
    uint8_t b_start = getBlue(beginColor);
    uint8_t r_end = getRed(endColor);
    uint8_t g_end = getGreen(endColor);
    uint8_t b_end = getBlue(endColor);

    // ????????????
    int dr = r_end - r_start;
    int dg = g_end - g_start;
    int db = b_end - b_start;

    // ?????
    uint16_t steps = (duration / 10) + 1; // ?10????
    uint16_t delay_time = duration / steps; // ???????

    // ????
    for (uint16_t step = 0; step <= steps; step++) // ?????????????????
    {
        // ????????
        float t = (float)step / steps; // ?????????????
        uint8_t r = r_start + (dr * t);
        uint8_t g = g_start + (dg * t);
        uint8_t b = b_start + (db * t);

        // ?? LED ??
        for (uint16_t i = 0; i < num; i++)
        {
            WS2812_Set_RGB(rgb_led, r, g, b, indexes[i]);
        }

        // ????
        WS2812_Show(rgb_led);

        // ??
        if (step < steps) // ??????????
        {
            rt_thread_mdelay(delay_time);
        }
    }
}