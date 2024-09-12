#ifndef __RGB_H
#define __RGB_H
#include "main.h"
#define Hight_Data            ( 64  )                           //1 ??????
#define Low_Data              ( 36  )                           //0 ??????
#define Reste_Data            ( 55  )                           //80 ?????????
#define Led_Num               (  1  )                           //WS2812????
#define Led_Data_Len          ( 24  )                           //WS2812????,????24???

 
//uint16_t RGB_buffur[Reste_Data + WS2812_Data_Len] = { 0 }; //??????
typedef struct
 {
    int                 led_num     ;
    TIM_HandleTypeDef   *htim       ;
    uint32_t            TIM_CHANNEL ;
    uint16_t*           RGB_buffer  ;
 }RGB_LED;

void RGB_LED_Init(RGB_LED *rgb_led,int led_num,TIM_HandleTypeDef *htim,uint32_t TIM_CHANNEL);
void WS2812_Set_Color(RGB_LED *rgb_led,uint32_t Color, uint16_t num);
void WS2812_Set_RGB(RGB_LED *rgb_led,uint8_t red, uint8_t green, uint8_t blue,uint16_t num);
void WS2812_Show(RGB_LED *rgb_led);
 
void colorGradient(RGB_LED *rgb_led, uint32_t beginColor, uint32_t endColor, uint16_t duration, int indexes[], int num);
#endif
 
 