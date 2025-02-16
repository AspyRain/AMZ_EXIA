#ifndef __GN_CONTROLLER_H
#define __GN_CONTROLLER_H

#include "RGB.h"
#include "easyflash.h"
#include <rtthread.h>

#define         LEG         0
#define         CHEST       1
#define         EYE         2
#define         HEAD        3
#define         ARM         4

#define         WEPON       5
#define         BIRD        0
#define         WINGS       1


void normal(RGB_LED *rgb_led,uint32_t color);
void chest_breathing(RGB_LED *rgb_led,uint32_t color);
void all_body_breathing(RGB_LED *rgb_led,uint32_t color);
void wavy_one(RGB_LED *rgb_led,uint32_t color8);
void wavy_two(RGB_LED *rgb_led,uint32_t color1,uint32_t color2);
void rainbow_RGB(RGB_LED *rgb_led);
void rainbow(RGB_LED *rgb_led, uint16_t duration, int indexes[], int num);
#endif