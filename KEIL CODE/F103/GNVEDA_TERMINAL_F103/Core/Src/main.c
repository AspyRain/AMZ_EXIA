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
#include "RGB.h"
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
STATE standby, day, night, temp; // 基础三个状态
STATE *current;                  // 当前状态
bool temp_enable = 0;
char usart3_c;
int usart3_rx_index;
char usart3_rx_buffer[BUFFER_SIZE];
int mode_id = 6;
int gn_mode_id = 0;
int command_able = 0;
uint32_t temp_color_1, temp_color_2;
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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  RGB_LED_Init(&rgb_test, 10, &htim1, TIM_CHANNEL_1);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  if (easyflash_init() == EF_NO_ERR) // Initialization Data Succeed
  {
    rt_kprintf("easyflash初始化成功!\n");
    data_init();
    //  uint32_t color1 = 0x00FF00;
    //  uint32_t color2 = 0xFFFFFF;
    //  rt_kprintf("要保存的数据:\n");
    //  print_hex_data(color1);
    //  rt_kprintf("\n");
    //  EfErrCode result = ef_set_env_blob("wavy_color_1", &color1,4);
    //  if (result == EF_NO_ERR)
    //  {
    //    rt_kprintf("数据1保存成功!\n");

    //   result = ef_set_env_blob("wavy_color_2", &color2,4);
    //   if (result == EF_NO_ERR)
    //   {
    //     rt_kprintf("数据2保存成功!\n");
    //   }
    // }
    // else
    // {
    //   rt_kprintf("数据保存失败!错误:%d\n", result);
    // }
  }

  HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_c, 1);

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
    rt_kprintf("rgb thread is not started\n");
  }
  processJsonThread = rt_thread_create("commandParsing", commandParsingTask, RT_NULL, 7216, 4, 10);
  if (processJsonThread != RT_NULL)
  {
    rt_thread_startup(processJsonThread);
  }

  // rt_thread_t light_task_tid = rt_thread_create("light_task_tid", /* ???? */
  //                                               light_detect, RT_NULL,
  //                                               512, 3, 10); //
  // if (light_task_tid != RT_NULL)
  // {
  //   rt_thread_startup(light_task_tid);
  //   rt_kprintf("light thread is already started\n");
  // }
  // else
  // {
  //   rt_kprintf("light thread is not started\n");
  // }
  // rt_thread_t temp_task_tid = rt_thread_create("temp_task_tid", /* ???? */
  //                                              temp_control, RT_NULL,
  //                                              512, 3, 10); //

  // if (temp_task_tid != RT_NULL)
  // {
  //   rt_thread_startup(temp_task_tid);
  //   rt_kprintf("temp thread is already started\n");
  // }
  // else
  // {
  //   rt_kprintf("temp thread is not started\n");
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

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
void loadStateFromFlash(STATE *state, const char *envName)
{
  if (ef_get_env_blob(envName, state, sizeof(STATE), NULL) == EF_NO_ERR)
  {
    rt_kprintf("结构体读取成功！\n");
  }
  else
  {
    rt_kprintf("结构体读取失败！\n");
  }
}

void printState(const STATE *state)
{
  rt_kprintf("State Name :%s,State ID: %d, Mode ID: %d, GN Mode ID: %d\n", state->state_env, state->state_id, state->mode_id, state->gn_mode_id);
}

void saveStateToFlash(const STATE *state, const char *envName)
{
  if (ef_set_env_blob(envName, state, sizeof(STATE)) == EF_NO_ERR)
  {
    rt_kprintf("结构体保存成功！\n");
  }
  else
  {
    rt_kprintf("结构体保存失败！\n");
  }
}
// 数据初始化
void data_init()
{

  // STATE save_standby = {"standby",1, 0, 0};
  // STATE save_day = {"day",2, 0, 0};
  // STATE save_night = {"night",3, 0, 0};

  // // 保存结构体到 Flash
  // saveStateToFlash(&save_standby, "state_standby");
  // saveStateToFlash(&save_day, "state_day");
  // saveStateToFlash(&save_night, "state_night");
  // // 读取并检测结构体
  // STATE loaded_standby;
  // STATE loaded_day;
  // STATE loaded_night;

  // loadStateFromFlash(&loaded_standby, "state_standby");
  // printState(&loaded_standby);

  // loadStateFromFlash(&loaded_day, "state_day");
  // printState(&loaded_day);

  // loadStateFromFlash(&loaded_night, "state_night");
  // printState(&loaded_night);

  loadStateFromFlash(&standby, "state_standby");
  printState(&standby);

  loadStateFromFlash(&day, "state_day");
  printState(&day);

  loadStateFromFlash(&night, "state_night");
  printState(&night);
}
void light_detect(void *promt)
{
  while (1)
  {
    if (!temp_enable)
    {
      current = &day;
    }
    rt_thread_mdelay(10);
  }
}
// 控制运行状态
void temp_control(void *promt)
{
  int temp_times = 0;
  while (1)
  {
    if (temp_enable)
    {
      current = &temp;
      temp_times++;
      rt_thread_mdelay(1);
    }
    else
    {
      temp = *current;
      temp_times = 0;
      rt_thread_mdelay(100);
    }
  }
}

void print_hex_data(uint32_t value)
{
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%06X", value);
  rt_kprintf("0x%s\n", buffer);
}

void rgb_task(void *promt)
{
  int times = 0;
  while (1)
  {
    switch (current->mode_id)
    {
    case 1:
    {
      uint32_t chest_color;
      if (temp_enable)
        chest_color = temp_color_1;
      else
      {
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_chest_color", current->state_env);
        ef_get_env_blob(envName, &chest_color, 4, NULL);
      }
      chest_breathing(&rgb_test, chest_color);
      break;
    }
    case 2:
    {
      uint32_t normal_color;
      if (temp_enable)
        normal_color = temp_color_1;
      else
      {
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_normal_color", current->state_env);
        ef_get_env_blob(envName, &normal_color, 4, NULL);
      }
      normal(&rgb_test, normal_color);
      break;
    }
    case 3:
    {
      uint32_t body_color;
      if (temp_enable)
        body_color = temp_color_1;
      else
      {
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_body_color", current->state_env);
        ef_get_env_blob(envName, &body_color, 4, NULL);
      }
      all_body_breathing(&rgb_test, body_color);
      break;
    }
    case 4:
    {
      uint32_t wavy_color_1;
      if (temp_enable)
        wavy_color_1 = temp_color_1;
      else
      {
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_wavy_color_1", current->state_env);
        ef_get_env_blob(envName, &wavy_color_1, 4, NULL);
      }
      wavy_one(&rgb_test, wavy_color_1);
      break;
    }
    case 5:
    {
      uint32_t wavy_color_1;
      uint32_t wavy_color_2;
      if (temp_enable)
      {
        wavy_color_1 = temp_color_1;
        wavy_color_2 = temp_color_2;
      }
      else
      {
        char envName[30]; // 足够长的缓冲区来存储完整的环境变量名称
        snprintf(envName, sizeof(envName), "%s_wavy_color_1", current->state_env);
        ef_get_env_blob(envName, &wavy_color_1, 4, NULL);
        snprintf(envName, sizeof(envName), "%s_wavy_color_2", current->state_env);
        ef_get_env_blob(envName, &wavy_color_2, 4, NULL);
      }
      wavy_two(&rgb_test, wavy_color_1, wavy_color_2);
      break;
    }
    case 6:
    {
      rainbow_RGB(&rgb_test);
      break;
    }
    }
    rt_thread_mdelay(100);
    // if (mode_id == 6){
    //   mode_id = 1;
    // }
    // else mode_id++;
    /* USER CODE BEGIN 3 */
  }
}
// ?????????????
uint32_t color_set;
char color_get[16]; // ??????????????

void setColorToEnv(const char *colors, const char *envName)
{
  rt_kprintf("setColorToEnv\n");
  int parsedItems = sscanf(colors, "[#%6s]", color_get);
  if (parsedItems == 1)
  {
    color_set = (strtol(color_get, NULL, 16));
    rt_kprintf("提取颜色0x%06X完成%s\n", color_set, envName);
    ef_set_env_blob(envName, &color_set, 4);
  }
}

// ?????????
void setDualColorToEnv(const char *colors, const char *envName1, const char *envName2)
{
  rt_kprintf("输入的颜色字符串: %s\n", colors);

  char color_get[7];
  char color_get_2[7];
  int parsedItems = sscanf(colors, "[#%6s, #%6s]", color_get, color_get_2);

  if (parsedItems == 2)
  {
    uint32_t color_set = (strtol(color_get, NULL, 16));
    rt_kprintf("提取颜色0x%06X完成,保存到 %s\n", color_set, envName1);
    ef_set_env_blob(envName1, &color_set, 4);

    color_set = (strtol(color_get_2, NULL, 16));
    rt_kprintf("提取颜色0x%06X完成,保存到 %s\n", color_set, envName2);
    ef_set_env_blob(envName2, &color_set, 4);
  }
  else
  {
    rt_kprintf("解析颜色字符串失败！\n");
  }
}

// ???
void processColor(const Message *message)
{

  switch ((int)message->mode.id)
  {
  case 1:
  {
    setColorToEnv(message->mode.colors, "chest_color");
    break;
  }
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
    // rt_kprintf("识别指令运行中!\n");
    if (command_able == 1)
    {
      rt_kprintf(usart3_rx_buffer);
      int parsedItems;
      int oId;
      char color_get[16];
      char color_get_2[16];
      uint32_t color_set = 0X000000;
      Message message;
      if (strcmp(usart3_rx_buffer, "get") == 0)
      {
        // Create a JSON object
        cJSON *root = cJSON_CreateObject();
        parseInitData(root,current);
        // Convert the JSON object to a string
        char *json_str = cJSON_PrintUnformatted(root);

        // Send the JSON string via USART
        sendData(&huart3, json_str);
        rt_kprintf(json_str);
        free(json_str);
        // Free the JSON string and object
        cJSON_Delete(root);
      }
      else if (parseMessage(usart3_rx_buffer, &message))
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart == &huart3)
  {
    if (usart3_c == '+')
    {
      clearUsart();
    }
    else if (usart3_c == '\n')
    {
      rt_kprintf("收到指令%s\n", usart3_rx_buffer);
      command_able = 1;
    }
    else
    {
      usart3_rx_buffer[usart3_rx_index++] = usart3_c;
    }
    HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_c, 1);
  }
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
