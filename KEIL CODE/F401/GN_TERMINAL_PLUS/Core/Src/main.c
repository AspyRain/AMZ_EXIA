/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rtthread.h>

#include "easyflash.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "gn_controller.h"
#include "jsonUtil.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 250
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RGB_LED rgb_test;
char usart3_c;
int usart3_rx_index;
char usart3_rx_buffer[BUFFER_SIZE];
int mode_id = 6;
int gn_mode_id = 0;
int command_able = 0;
rt_thread_t processJsonThread;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t color;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  RGB_LED_Init(&rgb_test, 30, &htim1, TIM_CHANNEL_1);
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  uint32_t Reboot_Time = 0;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  /* USER CODE BEGIN 2 */
  if (easyflash_init() == EF_NO_ERR) // Initialization Data Succeed
  {
    uint32_t color1 = 0xF0F0F0;
    uint32_t color2 = 0x0000FF;
    rt_kprintf("要保存的数据:\n");
    print_hex_data(color1);
    rt_kprintf("\n");
    EfErrCode result = ef_set_env_blob("wavy_color_1", &color1,4);
    if (result == EF_NO_ERR)
    {
      rt_kprintf("数据1保存成功!\n");
      
      result = ef_set_env_blob("wavy_color_2", &color2,4);
      if (result == EF_NO_ERR)
      {
        rt_kprintf("数据2保存成功!\n");
      }
    }
    else
    {
      rt_kprintf("数据保存失败!错误码:%d\n", result);
    }
  }

  HAL_UART_Receive_IT(&huart6, (uint8_t *)&usart3_c, 1);

  rt_thread_t rgb_task_tid = rt_thread_create("rgb_demo", /* ???? */
                                              rgb_task, RT_NULL,
                                              1024, 3, 10); //
  if (rgb_task_tid != RT_NULL)
  {
    rt_thread_startup(rgb_task_tid);
    rt_kprintf("rgb thread is already started\n");
  }
  else
  {
    rt_kprintf("usart thread is not started\n");
  }
  // processJsonThread = rt_thread_create("commandParsing", commandParsingTask, RT_NULL, 7216, 4, 10);
  // if (processJsonThread != RT_NULL)
  // {
  //   // ????
  //   rt_thread_startup(processJsonThread);
  // }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void print_hex_data(uint32_t value) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%06X", value);
    rt_kprintf("0x%s\n", buffer);
}

void rgb_task(void *promt)
{
  int times = 0;
  while (1)
  {
    switch (mode_id)
    {
    case 1:
    {
      chest_breathing(&rgb_test);
      break;
    }
    case 2:
    {
      normal(&rgb_test);
      break;
    }
    case 3:
    {
      all_body_breathing(&rgb_test);
      break;
    }
    case 4:
    {
      wavy_one(&rgb_test);
      break;
    }
    case 5:
    {
      wavy_two(&rgb_test);
      break;
    }
    case 6:
    {
      rainbow_RGB(&rgb_test);
      break;
    }
    }
    rt_kprintf("rgb 线程运行:%d次\n", ++times);
    rt_thread_mdelay(100);

    /* USER CODE BEGIN 3 */
  }
}
// ?????????????
uint32_t color_set;
char color_get[16]; // ??????????????

void setColorToEnv(const char *colors, const char *envName)
{
  int parsedItems = sscanf(colors, "[#%6s]", color_get);
  if (parsedItems == 1)
  {
    color_set = strtol(color_get, NULL, 16);
    rt_kprintf("提取颜色0x%06X完成%s\n", color_set, envName);
    ef_set_env_blob(envName, &color_set, 4);
  }
}

// ?????????
void setDualColorToEnv(const char *colors, const char *envName1, const char *envName2)
{
  char color_get_2[7];
  int parsedItems = sscanf(colors, "[#%6s,#%6s]", color_get, color_get_2);
  if (parsedItems == 2)
  {
    color_set = strtol(color_get, NULL, 16);
    ef_set_env_blob(envName1, &color_set, 4);
    color_set = strtol(color_get_2, NULL, 16);
    ef_set_env_blob(envName2, &color_set, 4);
  }
}
// ???
void processColor(const Message *message)
{
  switch (message->mode.id)
  {
  case 1:
    setColorToEnv(message->mode.colors, "chest_color");
    break;
  case 2:
    setColorToEnv(message->mode.colors, "normal_color");
    break;
  case 3:
    setColorToEnv(message->mode.colors, "body_color");
    break;
  case 4:
    setColorToEnv(message->mode.colors, "wavy_color_1");
    break;
  case 5:
    setDualColorToEnv(message->mode.colors, "wavy_color_1", "wavy_color_2");
    break;
  default:
    // Handle other cases or do nothing
    break;
  }
}
void commandParsingTask(void *prmt)
{
  while (1)
  {
    if (command_able == 1)
    {
      rt_kprintf("??????%s\n", usart3_rx_buffer);
      int parsedItems;
      int oId;
      char color_get[16];
      char color_get_2[16];
      uint32_t color_set = 0X000000;
      Message message;
      if (parseMessage(usart3_rx_buffer, &message))
      {
        rt_kprintf("Mode Type: %d\n", message.modeType);
        rt_kprintf("Mode ID: %d, Colors: %s\n", message.mode.id, message.mode.colors);
        rt_kprintf("gnMode ID: %d, Colors: %s\n", message.gnMode.id, message.gnMode.colors);
        mode_id = message.mode.id;
        processColor(&message);
      }
      else
      {
        rt_kprintf("Failed to parse the JSON string.\n");
      }

      // Free allocated memory
      free(message.mode.colors);
      free(message.gnMode.colors);
      command_able = 0;
    }
    else
      rt_thread_mdelay(10);
  }
}
void clearUsart()
{
  memset(usart3_rx_buffer, 0, sizeof(usart3_rx_buffer));
  usart3_rx_index = 0;
}
/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM3 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM3)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
