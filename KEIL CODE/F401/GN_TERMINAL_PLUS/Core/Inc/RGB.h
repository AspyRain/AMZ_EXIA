#ifndef __RGB_H
#define __RGB_H

#include "main.h"

// 定义高电平数据的值
#define Hight_Data                        (68)  
// 定义低电平数据的值
#define Low_Data                           (34)  
// 定义用于存放LED数据的数组大小
#define Reset_Data                       (55)  
// 定义使用的LED灯串的数量
#define Leds_Num                             (1)    
// 每个LED的数据长度（对于WS2812来说是24位）
#define Led_Data_Len                   (24)   

// RGB_LED结构体，包含控制LED所需的信息
typedef struct
{
    int led_num;             // LED的数量
    TIM_HandleTypeDef *htim; // 用于PWM输出的定时器句柄
    uint32_t TIM_CHANNEL;    // 定时器通道
    uint16_t *RGB_buffer;    // 存储LED颜色数据的缓冲区
} RGB_LED;

// 初始化RGB_LED结构体
void RGB_LED_Init(RGB_LED *rgb_led, int led_num, TIM_HandleTypeDef *htim, uint32_t TIM_CHANNEL);

// 设置指定索引位置的LED颜色（通过颜色值）
void WS2812_Set_Color(RGB_LED *rgb_led, uint32_t Color, uint16_t index);

// 设置指定索引位置的LED颜色（通过RGB三文色值）
void WS2812_Set_RGB(RGB_LED *rgb_led, uint8_t red, uint8_t green, uint8_t blue, uint16_t index);

// 更新所有LED的颜色显示
void WS2812_Show(RGB_LED *rgb_led);

// 创建一个颜色渐变效果，从开始颜色到结束颜色，持续一定时间
void colorGradient(RGB_LED *rgb_led, uint32_t beginColor, uint32_t endColor, uint16_t duration, int indexes[], int num);

#endif // __RGB_H