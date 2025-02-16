#ifndef EF_CFG_H_
#define EF_CFG_H_

/* 使用ENV功能，默认是从V4.0开始的新一代模式 */
#define EF_USING_ENV

#ifdef EF_USING_ENV
/* 当前ENV版本号更改时，自动更新ENV到最新默认设置 */
/* #define EF_ENV_AUTO_UPDATE */
#define EF_ENV_VER_NUM         (0)    /* ENV版本号 */

/* MCU字节序配置，默认是小端字节序 */
/* #define EF_BIG_ENDIAN */      
#endif /* EF_USING_ENV */

/* 使用IAP（In-Application Programming）功能 */
/* #define EF_USING_IAP */

/* 使用日志记录功能 */
/* #define EF_USING_LOG */

/* Flash擦除的最小单位 */
#define STM_SECTOR_SIZE     (2048)    /* 2K字节 */
#define EF_ERASE_MIN_SIZE       (STM_SECTOR_SIZE)  /* 必须定义的一个值 */


/* Flash写入粒度，单位：位
 * 只支持1（Nor Flash）/ 8（STM32F4）/ 32（STM32F1） */
#define EF_WRITE_GRAN           (32) /* 必须定义的一个值 */

/* 用于读取ENV和继续查找地址函数的缓冲区大小 */
#define EF_READ_BUF_SIZE             32     /* 默认32，更大的数值可以提高首次分配ENV的速度，但需要更多的栈空间 */

/* 备份区域的起始地址 */
#define STM32_FLASH_BASE    0x08000000UL     /* STM32 Flash的起始地址 */


#define EF_START_ADDR          (STM32_FLASH_BASE+STM_SECTOR_SIZE*64)   /* 必须定义的一个值 */

/* ENV区域的大小。至少需要一个空扇区用于垃圾回收。所以它的定义必须大于等于2个扇区大小。 */
#define ENV_AREA_SIZE          (2 * EF_ERASE_MIN_SIZE)  /* 如果使用ENV，则必须定义一个值 */

/* 日志存储区域的大小 */
#define LOG_AREA_SIZE          (10 * EF_ERASE_MIN_SIZE)  /* 如果使用日志，则必须定义一个值 */

/* 打印Flash的相关调试信息 */
#define PRINT_DEBUG

#endif /* EF_CFG_H_ */