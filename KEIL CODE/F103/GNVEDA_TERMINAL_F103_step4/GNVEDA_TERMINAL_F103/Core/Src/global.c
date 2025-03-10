#include "global.h"
#include "easyflash.h"
#include <rtthread.h>
uint32_t basic_color;
uint32_t basic_color;
uint32_t chest_color;
uint32_t body_color;
uint32_t wavy_color_1;
uint32_t wavy_color_2;
uint32_t circle_color_1;
uint32_t circle_color_2;
uint32_t center_color;
uint32_t wings_color;
uint32_t bird_color;

static uint32_t get_color_from_easyflash(const char *key, uint32_t default_value) {
    uint32_t value;
    size_t len = sizeof(value);
    
    if (ef_get_env_blob(key, &value, len, NULL) <= 0) {
        // 若获取失败，则使用默认值
        value = default_value;
        rt_kprintf("获取颜色 %s 失败，使用默认值 0x%06X\n", key, value);
        ef_set_env_blob(key, &value, len);
    } else {
    
        rt_kprintf("成功获取颜色 %s: 0x%06X\n", key, value);
    }

    return value;
}


void init_color(void) {

    chest_color   = get_color_from_easyflash("chest_color",   0xFF0000);
    body_color    = get_color_from_easyflash("body_color",    0xFF0000);
    wavy_color_1  = get_color_from_easyflash("wavy_color_1",  0xFF0000);
    wavy_color_2  = get_color_from_easyflash("wavy_color_2",  0x00FF00);  // 默认红色
    circle_color_1 = get_color_from_easyflash("circle_color_1", 0xFF0000);
    circle_color_2 = get_color_from_easyflash("circle_color_2", 0xFF0000);
    center_color  = get_color_from_easyflash("center_color",  0xFF0000);
    wings_color   = get_color_from_easyflash("wings_color",   0x0000FF);  // 默认蓝色
    bird_color    = get_color_from_easyflash("bird_color",    0xFF0000);
    basic_color  = get_color_from_easyflash("basic_color",  0xFF0000);  // 默认绿色
}
