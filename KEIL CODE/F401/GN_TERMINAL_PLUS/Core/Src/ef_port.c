/*
 * This file is part of the EasyFlash Library.
 *
 * Copyright (c) 2015-2019, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-01-16
 */
 
#include <easyflash.h>
#include <stdarg.h>
#include "main.h"
#include <rtthread.h>
uint32_t red=0x00FF00;
uint32_t green=0xFF0000;
uint32_t blue=0x0000FF;
/* default environment variables set for user */
static const ef_env default_env_set[] = {
        {"normal_color" ,"0"  },
        {"chest_color"  ,"0"  },
        {"body_color"   ,"0"  },
        {"wavy_color_1" ,"0"  },
        {"wavy_color_2" ,"0"  },
		{"reboot_time", "\0"}		//Used to store startup times
};
 
/**
 * Flash port for hardware initialize.
 *
 * @param default_env default ENV set for user
 * @param default_env_size default ENV size
 *
 * @return result
 */
EfErrCode ef_port_init(ef_env const **default_env, size_t *default_env_size) {
    rt_kprintf("执行init\n");
    EfErrCode result = EF_NO_ERR;
 
    *default_env = default_env_set;
    *default_env_size = sizeof(default_env_set) / sizeof(default_env_set[0]);
 
    return result;
}
 
/**
 * Read data from flash.
 * @note This operation's units is word.
 *
 * @param addr flash address
 * @param buf buffer to store read data
 * @param size read bytes size
 *
 * @return result
 */
EfErrCode ef_port_read(uint32_t addr, uint32_t *buf, size_t size) {
    //rt_kprintf("执行read\n");
    EfErrCode result = EF_NO_ERR;
    
    // 确保地址是对齐的，并且数据长度是4字节的倍数
    EF_ASSERT((addr % 4) == 0);
    EF_ASSERT((size % 4) == 0);

    uint8_t *buf_8 = (uint8_t *)buf;
    size_t i;
    for (i = 0; i < size; i += 4, addr += 4, buf_8 += 4) {
        // 直接读取32位数据
        *buf_8 = *(uint8_t *)addr;
        *(buf_8 + 1) = *(uint8_t *)(addr + 1);
        *(buf_8 + 2) = *(uint8_t *)(addr + 2);
        *(buf_8 + 3) = *(uint8_t *)(addr + 3);
    }
    return result;
}
 
// 计算扇区号
uint32_t calculate_sector_number(uint32_t addr) {
    // 检查地址是否在第一个扇区范围内
    if (addr < STM32_SECTOR_5_START) {
        // 前4个扇区，每个16KB
        return (addr - STM32_FLASH_BASE) / STM_SECTOR_SIZE_16KB;
    } else if (addr < STM32_SECTOR_6_START) {
        // 第5个扇区，64KB
        return 4 + (addr - STM32_SECTOR_5_START) / STM_SECTOR_SIZE_64KB;
    } else {
        // 第6至第11个扇区，每个128KB
        return 5 + (addr - STM32_SECTOR_6_START) / STM_SECTOR_SIZE_128KB;
    }
}

// 计算需要擦除的扇区数量
uint32_t calculate_sectors_to_erase(uint32_t start_addr, uint32_t end_addr) {
    uint32_t start_sector = calculate_sector_number(start_addr);
    uint32_t end_sector = calculate_sector_number(end_addr - 1);
    return end_sector - start_sector + 1;
}

/**
 * Erase data on flash.
 * @note This operation is irreversible.
 * @note This operation's units is different which on many chips.
 *
 * @param addr flash address
 * @param size erase bytes size
 *
 * @return result
 */

EfErrCode ef_port_erase(uint32_t addr, size_t size) {
    rt_kprintf("执行erase\n");
    rt_kprintf("擦除起始地址: 0x%08X\n", addr);
    rt_kprintf("擦除大小: %lu 字节\n", size);

    EfErrCode result = EF_NO_ERR;

    // 确保起始地址是EF_ERASE_MIN_SIZE的倍数
    if (addr % EF_ERASE_MIN_SIZE != 0) {
        rt_kprintf("错误：起始地址不是EF_ERASE_MIN_SIZE的倍数！\n");
        return EF_ERASE_ERR;
    }

    // 解锁Flash访问
    rt_kprintf("解锁Flash...\n");
    HAL_FLASH_Unlock();

    // 计算需要擦除的起始和结束地址
    uint32_t start_addr = addr;
    uint32_t end_addr = addr + size - 1;

    // 初始化擦除结构体
    FLASH_EraseInitTypeDef FlashSet = {0};
    FlashSet.TypeErase = FLASH_TYPEERASE_SECTORS; // 扇区擦除
    FlashSet.Banks = FLASH_BANK_1;                // 使用默认的Bank 1
    FlashSet.Sector = calculate_sector_number(start_addr);   // 根据地址计算出对应的扇区号
    FlashSet.NbSectors = calculate_sectors_to_erase(start_addr, end_addr); // 计算需要擦除的扇区数量
    FlashSet.VoltageRange = FLASH_VOLTAGE_RANGE_3; // 根据设备电压范围选择适当的电压范围

    rt_kprintf("擦除配置:\n");
    rt_kprintf("  TypeErase: %d\n", FlashSet.TypeErase);
    rt_kprintf("  Banks: %d\n", FlashSet.Banks);
    rt_kprintf("  Sector: %d\n", FlashSet.Sector);
    rt_kprintf("  NbSectors: %d\n", FlashSet.NbSectors);
    rt_kprintf("  VoltageRange: %d\n", FlashSet.VoltageRange);

    // 擦除Flash
    uint32_t PageError = 0;
    rt_kprintf("开始擦除...\n");
    if (HAL_FLASHEx_Erase(&FlashSet, &PageError) != HAL_OK) {
        rt_kprintf("擦除失败！错误页号: %d\n", PageError);
        result = EF_ERASE_ERR;
    } else {
        rt_kprintf("擦除成功！\n");
    }

    // 锁定Flash访问
    rt_kprintf("锁定Flash...\n");
    HAL_FLASH_Lock();

    return result;
}



/**
 * Write data to flash.
 * @note This operation's units is word.
 * @note This operation must after erase. @see flash_erase.
 *
 * @param addr flash address
 * @param buf the write data buffer
 * @param size write bytes size
 *
 * @return result
 */
EfErrCode ef_port_write(uint32_t addr, const uint32_t *buf, size_t size) {
    EfErrCode result = EF_NO_ERR;
    /* 解锁Flash访问 */
    rt_kprintf("解锁Flash...\n");
    HAL_FLASH_Unlock();
    __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPERR);

    /* 写入数据 */
    rt_kprintf("开始写入数据...\n");
    for (size_t i = 0; i < size; i += 4, buf++, addr += 4) {
        /* 写入数据 */
        rt_kprintf("写入数据：");
        print_hex_data(*buf);
        rt_kprintf(" 到地址 0x%08X\n", addr);
        // if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *buf) != HAL_OK) {
        //     rt_kprintf("写入失败！\n");
        //     result = EF_WRITE_ERR;
        //     break;
        // }
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr, *buf);
        /* 读取数据进行校验 */
        uint32_t read_data = *(uint32_t *)addr;
        rt_kprintf("读取数据：");
        print_hex_data(read_data);
        rt_kprintf("\n");
        if (read_data != *buf) {
            rt_kprintf("数据校验失败！\n");
            result = EF_WRITE_ERR;
            break;
        }
    }

    /* 锁定Flash访问 */
    rt_kprintf("锁定Flash...\n");
    HAL_FLASH_Lock();

    return result;
}

/**
 * lock the ENV ram cache
 */
void ef_port_env_lock(void) {
    
    /* You can add your code under here. */
    
}
 
/**
 * unlock the ENV ram cache
 */
void ef_port_env_unlock(void) {
    
    /* You can add your code under here. */
    
}
 
 
/**
 * This function is print flash debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 *
 */
void ef_log_debug(const char *file, const long line, const char *format, ...) {
 
#ifdef PRINT_DEBUG
 
    va_list args;
 
    /* args point to the first variable parameter */
    va_start(args, format);
 
    /* You can add your code under here. */
    
    va_end(args);
 
#endif
 
}
 
/**
 * This function is print flash routine info.
 *
 * @param format output format
 * @param ... args
 */
void ef_log_info(const char *format, ...) {
    va_list args;
 
    /* args point to the first variable parameter */
    va_start(args, format);
 
    /* You can add your code under here. */
    
    va_end(args);
}
/**
 * This function is print flash non-package info.
 *
 * @param format output format
 * @param ... args
 */
void ef_print(const char *format, ...) {
    va_list args;
 
    /* args point to the first variable parameter */
    va_start(args, format);
 
    /* You can add your code under here. */
    
    va_end(args);
}