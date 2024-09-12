#ifndef EF_CFG_H_
#define EF_CFG_H_

/* using ENV function, default is NG (Next Generation) mode start from V4.0 */
#define EF_USING_ENV

#ifdef EF_USING_ENV
/* Auto update ENV to latest default when current ENV version number is changed. */
/* #define EF_ENV_AUTO_UPDATE */
#define EF_ENV_VER_NUM         (0)

/* MCU Endian Configuration, default is Little Endian Order. */
/* #define EF_BIG_ENDIAN  */          
#endif /* EF_USING_ENV */

/* using IAP function */
/* #define EF_USING_IAP */

/* using save log function */
/* #define EF_USING_LOG */

/* The minimum size of flash erasure. May be a flash sector size. */
#define STM_SECTOR_SIZE     1024    //1K字节
#define EF_ERASE_MIN_SIZE       (STM_SECTOR_SIZE)  /* @note you must define it for a value */

/* the flash write granularity, unit: bit
 * only support 1(nor flash)/ 8(stm32f4)/ 32(stm32f1) */
#define EF_WRITE_GRAN           (8) /* @note you must define it for a value */

/* The size of read_env and continue_ff_addr function used*/
#define EF_READ_BUF_SIZE             32     /* @default 32, Larger numbers can improve first-time speed of alloc_env but require more stack space*/

/* backup area start address */
#define STM32_FLASH_BASE    0x08000000UL 		//STM32 FLASH的起始地址
#define EF_START_ADDR          (STM32_FLASH_BASE+EF_ERASE_MIN_SIZE*62)   /* @note you must define it for a value */

/* ENV area size. It's at least one empty sector for GC. So it's definition must more then or equal 2 flash sector size. */
#define ENV_AREA_SIZE          (2 * EF_ERASE_MIN_SIZE)  /* @note you must define it for a value if you used ENV */

/* saved log area size */
#define LOG_AREA_SIZE          (10 * EF_ERASE_MIN_SIZE)  /* @note you must define it for a value if you used log */

/* print debug information of flash */
#define PRINT_DEBUG

#endif /* EF_CFG_H_ */