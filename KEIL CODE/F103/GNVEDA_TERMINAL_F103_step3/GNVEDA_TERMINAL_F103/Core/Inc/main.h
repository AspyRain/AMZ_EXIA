/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void rgb_body_task(void *promt);
void commandParsingTask(void *prmt);
void clearUsart();
void print_hex_data(uint32_t value);
void rgb_gn_drive_task(void *promt);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TEST_TX_Pin GPIO_PIN_2
#define TEST_TX_GPIO_Port GPIOA
#define TEST_RX_Pin GPIO_PIN_3
#define TEST_RX_GPIO_Port GPIOA
#define BRIGHT_IN_Pin GPIO_PIN_4
#define BRIGHT_IN_GPIO_Port GPIOA
#define BT_TX_Pin GPIO_PIN_10
#define BT_TX_GPIO_Port GPIOB
#define BT_RX_Pin GPIO_PIN_11
#define BT_RX_GPIO_Port GPIOB
#define RGB_BODY_Pin GPIO_PIN_8
#define RGB_BODY_GPIO_Port GPIOA
#define LIGHT_OUT_Pin GPIO_PIN_10
#define LIGHT_OUT_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
