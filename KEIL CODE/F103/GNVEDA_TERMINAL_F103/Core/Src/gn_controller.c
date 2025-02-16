#include "gn_controller.h"
int gnDrive[] = {2, 3, 4, 5, 6, 7, 8};

#define RED           0xFF0000
#define ORANGE        0xFFA500
#define YELLOW        0xFFFF00
#define GREEN         0x00FF00
#define CYAN          0x00FFFF
#define BLUE          0x0000FF
#define PURPLE        0x800080

// 定义彩虹颜色数组
uint32_t rainbow_colors[] = {
    RED,
    ORANGE,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    PURPLE
};
uint32_t rgb_to_grb(uint32_t rgb) {
    // 提取 R、G、B 分量
    uint8_t r = (rgb >> 16) & 0xFF;
    uint8_t g = (rgb >> 8) & 0xFF;
    uint8_t b = rgb & 0xFF;

    // 组合成 GRB 颜色值
    uint32_t grb = (g << 16) | (r << 8) | b;

    return grb;
}

void normal(RGB_LED *rgb_led,uint32_t color)
{   
    resetColor(rgb_led,0,0);
    int body_indexes[] = {LEG, CHEST, EYE, HEAD, ARM};
    for (int i=0;i<5;i++){
        WS2812_Set_Color(rgb_led,color,body_indexes[i]);
        //WS2812_Set_Color(rgb_led,normal_color,body_indexes[i]);
        WS2812_Show(rgb_led);
    }
    rt_thread_mdelay(500);
}

void chest_breathing(RGB_LED *rgb_led,uint32_t color)
{
    resetColor(rgb_led,0,0);
    int chest_indexes[] = {CHEST};
    colorGradient(rgb_led, 0X000000, color, 2000, chest_indexes, 1);
    colorGradient(rgb_led, color, 0X000000, 2000, chest_indexes, 1);
    rt_thread_mdelay(500);
}

void all_body_breathing(RGB_LED *rgb_led,uint32_t color)
{
    resetColor(rgb_led,0,0);
    int body_indexes[] = {LEG, CHEST, EYE, HEAD, ARM};
    colorGradient(rgb_led, 0X000000, color, 2000, body_indexes, 5);
    colorGradient(rgb_led, color, 0X000000, 2000, body_indexes, 5);
    rt_thread_mdelay(500);
}

void wavy_one(RGB_LED *rgb_led,uint32_t color)
{
    resetColor(rgb_led,0,0);
    int step_1[] = {CHEST};
    int step_2[] = {ARM};
    int step_3[] = {LEG, EYE};
    int step_4[] = {HEAD};
    colorGradient(rgb_led, 0X000000, color, 200, step_1, 1);
    colorGradient(rgb_led, 0X000000, color, 200, step_2, 1);
    colorGradient(rgb_led, 0X000000, color, 200, step_3, 2);
    colorGradient(rgb_led, 0X000000, color, 200, step_4, 1);
    rt_thread_mdelay(200);
    colorGradient(rgb_led, color, 0X000000, 200, step_1, 1);
    colorGradient(rgb_led, color, 0X000000, 200, step_2, 1);
    colorGradient(rgb_led, color, 0X000000, 200, step_3, 2);
    colorGradient(rgb_led, color, 0X000000, 200, step_4, 1);
    rt_thread_mdelay(200);
}

void wavy_two(RGB_LED *rgb_led,uint32_t color1,uint32_t color2)
{
    resetColor(rgb_led,0,0);
    int step_1[] = {CHEST};
    int step_2[] = {ARM};
    int step_3[] = {LEG, EYE};
    int step_4[] = {HEAD};
    colorGradient(rgb_led, 0X000000, color1, 200, step_1, 1);
    colorGradient(rgb_led, 0X000000, color1, 200, step_2, 1);
    colorGradient(rgb_led, 0X000000, color1, 200, step_3, 2);
    colorGradient(rgb_led, 0X000000, color1, 200, step_4, 1);
    rt_thread_mdelay(100);
    colorGradient(rgb_led, color1, color2, 200, step_1, 1);
    colorGradient(rgb_led, color1, color2, 200, step_2, 1);
    colorGradient(rgb_led, color1, color2, 200, step_3, 2);
    colorGradient(rgb_led, color1, color2, 200, step_4, 1);
    rt_thread_mdelay(200);
    colorGradient(rgb_led, color2, 0X000000, 200, step_1, 1);
    colorGradient(rgb_led, color2, 0X000000, 200, step_2, 1);
    colorGradient(rgb_led, color2, 0X000000, 200, step_3, 2);
    colorGradient(rgb_led, color2, 0X000000, 200, step_4, 1);
    rt_thread_mdelay(200);
}

void rainbow_RGB(RGB_LED *rgb_led){
    resetColor(rgb_led,0,0);
    int rainbow_indexes[] = {LEG, CHEST, EYE, HEAD, ARM};
		int rainbow_indexes_test[10];
		for (int i=0;i<10;i++){
			rainbow_indexes_test[i]=i;
		}
    rainbow(rgb_led,350,rainbow_indexes_test,10);
}

// 颜色数量
const int NUM_COLORS = sizeof(rainbow_colors) / sizeof(rainbow_colors[0]);

/**
 * @brief 执行炫彩渐变
 * @param rgb_led: RGB_LED 结构体指针
 * @param duration: 每次渐变的持续时间 (毫秒)
 * @param indexes: LED 索引数组
 * @param num: LED 数量
 */
void rainbow(RGB_LED *rgb_led, uint16_t duration, int indexes[], int num)
{		
    // 循环遍历彩虹颜色
    for (int color_idx = 0; color_idx < NUM_COLORS; color_idx++)
    {
        // 获取当前颜色和下一个颜色
        uint32_t current_color = rainbow_colors[color_idx];
        uint32_t next_color = rainbow_colors[(color_idx + 1) % NUM_COLORS];
        //print_hex_data(next_color);
        // 执行颜色渐变
        colorGradient(rgb_led, current_color, next_color, duration, indexes, num);
    }

}