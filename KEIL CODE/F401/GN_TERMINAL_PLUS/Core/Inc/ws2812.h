#ifndef WS2812_H
#define WS2812_H

#include "stm32f4xx_hal.h"

typedef struct
{
    int led_num;         // LED??
    TIM_HandleTypeDef *htim; // ?????
    uint32_t TIM_CHANNEL; // ?????
    uint16_t *RGB_buffer; // ????
} RGB_LED;

void RGB_LED_Init(RGB_LED *rgb_led, int led_num, TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL);
void WS2812_Set_Color(RGB_LED *rgb_led, uint32_t Color, uint16_t index);
void WS2812_Set_RGB(RGB_LED *rgb_led, uint8_t red, uint8_t green, uint8_t blue, uint16_t index);
void WS2812_Show(RGB_LED *rgb_led);

#endif // WS2812_H