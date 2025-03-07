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
#include <string.h>
#include <stdlib.h>
#include "global.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 250
#define TEST_ENABLE 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RGB_LED rgb_body;
RGB_LED rgb_bag;
char usart3_c;
int usart3_rx_index;
char usart3_rx_buffer[BUFFER_SIZE];
int mode_id = 6;

int gn_mode_id = 0;
int command_able = 0;
bool gn_reset = true;
rt_thread_t processJsonThread;
bool is_double = false, is_center = false;
int GN_DRIVE[] = {2, 3, 4, 5, 6, 7, 8};
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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  int times = 0;
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM5_Init();
  /* USER CODE BEGIN 2 */
  if (easyflash_init() == EF_NO_ERR) // Initialization Data Succeed
  {
    rt_kprintf("easyflash初始化成功!\n");
    ef_print_env();
    test_env();
    init_color();
    // uint32_t color1 = 0x00FF00;
    // uint32_t color2 = 0x0000FF;
    // ef_set_env_blob("bird_color", &color1, 4);
    // ef_set_env_blob("wings_color", &color2, 4);

    // rt_kprintf("要保存的数据:\n");
    // print_hex_data(color1);
    // rt_kprintf("\n");
    // EfErrCode result = ef_set_env_blob("wavy_color_1", &color1, 4);
    // if (result == EF_NO_ERR)
    // {
    //   rt_kprintf("数据1保存成功!\n");

    // }
    // else
    // {
    //   rt_kprintf("数据保存失败!错误:%d\n", result);
    // }
  }

  HAL_UART_Receive_IT(&huart3, (uint8_t *)&usart3_c, 1);

  if (TEST_ENABLE)
  {
    RGB_LED_Init(&rgb_body, 10, &htim1, TIM_CHANNEL_1);
    RGB_LED_Init(&rgb_bag, 10, &htim5, TIM_CHANNEL_1);

    rt_thread_t rgb_body_task_tid = rt_thread_create("rgb_body_task", /* ???? */
                                                     rgb_body_task, RT_NULL,
                                                     1024, 3, 10); //
    if (rgb_body_task_tid != RT_NULL)
    {
      rt_thread_startup(rgb_body_task_tid);
      rt_kprintf("rgb_body_task_tid thread is already started\n");
    }
    else
    {
      rt_kprintf("rgb_body_task_tid thread is not started\n");
    }

    rt_thread_t rgb_gn_task_tid = rt_thread_create("rgb_gn_drive_task", /* ???? */
                                                   rgb_gn_drive_task, RT_NULL,
                                                   1024, 4, 10); //
    if (rgb_gn_task_tid != RT_NULL)
    {
      rt_thread_startup(rgb_gn_task_tid);
      rt_kprintf("rgb_gn_task_tid thread is already started\n");
    }
    else
    {
      rt_kprintf("rgb_gn_task_tid thread is not started\n");
    }

    processJsonThread = rt_thread_create("commandParsing", commandParsingTask, RT_NULL, 7216, 4, 10);
    if (processJsonThread != RT_NULL)
    {
      rt_thread_startup(processJsonThread);
    }
  }
  else
  {
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    rt_thread_mdelay(20);
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
static void test_env(void)
{
  uint32_t i_boot_times = NULL;
  char *c_old_boot_times, c_new_boot_times[11] = {0};

  /* get the boot count number from Env */
  c_old_boot_times = ef_get_env("boot_times");
  assert_param(c_old_boot_times);
  i_boot_times = atol(c_old_boot_times);
  /* boot count +1 */
  i_boot_times++;
  rt_kprintf("The system now boot %d times\n\r", i_boot_times);
  /* interger to string */
  sprintf(c_new_boot_times, "%u", i_boot_times);
  /* set and store the boot count number to Env */
  ef_set_env("boot_times", c_new_boot_times);


  // uint32_t color1;
  // ef_get_env_blob("chest_color", &color1, 4,NULL);
  // rt_kprintf("chest_color: ");
  // print_hex_data(color1);
  // if (ef_set_env("basic_color", 0x00FF00)==EF_NO_ERR)
  // {
  //   rt_kprintf("basic_color set success\n");
  // }
  // else
  // {
  //   rt_kprintf("basic_color set failed\n");
  // }
  ef_save_env();
}

void print_hex_data(uint32_t value)
{
  char buffer[10];
  snprintf(buffer, sizeof(buffer), "%06X", value);
  rt_kprintf("0x%s\n", buffer);
}

void rgb_gn_drive_task(void *promt)
{
  int delay_time = 1000;
  int circle_num = 0;
  while (1)
  {
    resetColor(&rgb_bag, GN_DRIVE, 7);
    WS2812_Set_Color(&rgb_bag, bird_color, 1);
    WS2812_Set_Color(&rgb_bag, wings_color, 0);
    if (gn_reset)
    {
      delay_time = 1000;
      gn_reset = false;
    }
    if (is_center)
      WS2812_Set_Color(&rgb_bag, center_color, GN_DRIVE[6]);
    if (is_double)
      WS2812_Set_Color(&rgb_bag, circle_color_2, GN_DRIVE[circle_num + 3 >= 6 ? circle_num - 3 : circle_num + 3]);
    WS2812_Set_Color(&rgb_bag, circle_color_1, GN_DRIVE[circle_num]);
    circle_num++;
    if (circle_num >= 6)
    {
      circle_num = 0;
    }
    if (delay_time >= 10)
      delay_time = delay_time * 0.90;
    WS2812_Show(&rgb_bag);
    rt_thread_mdelay(delay_time);
  }
}

void rgb_body_task(void *promt)
{
  while (1)
  {
    switch (mode_id)
    {
    case 1:
    {
      chest_breathing(&rgb_body);
      break;
    }
    case 2:
    {
      normal(&rgb_body);
      break;
    }
    case 3:
    {
      all_body_breathing(&rgb_body);
      break;
    }
    case 4:
    {
      wavy_one(&rgb_body);
      break;
    }
    case 5:
    {
      wavy_two(&rgb_body);
      break;
    }
    case 6:
    {
      rainbow_RGB(&rgb_body);
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


// 更新全局颜色变量的映射
static void update_global_color(const char *envName, uint32_t color)
{
  if (strcmp(envName, "basic_color") == 0)
  {
    basic_color = color;
  }
  else if (strcmp(envName, "chest_color") == 0)
  {
    chest_color = color;
  }
  else if (strcmp(envName, "body_color") == 0)
  {
    body_color = color;
  }
  else if (strcmp(envName, "wavy_color_1") == 0)
  {
    wavy_color_1 = color;
  }
  else if (strcmp(envName, "wavy_color_2") == 0)
  {
    wavy_color_2 = color;
  }
  else if (strcmp(envName, "circle_color_1") == 0)
  {
    circle_color_1 = color;
  }
  else if (strcmp(envName, "circle_color_2") == 0)
  {
    circle_color_2 = color;
  }
  else if (strcmp(envName, "center_color") == 0)
  {
    center_color = color;
  }
  else if (strcmp(envName, "wings_color") == 0)
  {
    wings_color = color;
  }
  else if (strcmp(envName, "bird_color") == 0)
  {
    bird_color = color;
  }
}

void setColorToEnv(const char *colors, const char *envName)
{
  rt_kprintf("setColorToEnv\n");

  char color_get[7]; // 存储解析出的颜色
  int parsedItems = sscanf(colors, "[#%6s]", color_get);

  if (parsedItems == 1)
  {
    uint32_t color_set = strtol(color_get, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName);
    ef_set_env_blob(envName, &color_set, 4);

    // 更新全局变量
    update_global_color(envName, color_set);
  }
  else
  {
    rt_kprintf("解析颜色字符串失败！\n");
  }
}

void setDualColorToEnv(const char *colors, const char *envName1, const char *envName2)
{
  rt_kprintf("输入的颜色字符串: %s\n", colors);

  char color_get[7], color_get_2[7]; // 存储解析出的颜色
  int parsedItems = sscanf(colors, "[#%6s, #%6s]", color_get, color_get_2);

  if (parsedItems == 2)
  {
    uint32_t color_set = strtol(color_get, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName1);
    ef_set_env_blob(envName1, &color_set, 4);
    update_global_color(envName1, color_set);

    color_set = strtol(color_get_2, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName2);
    ef_set_env_blob(envName2, &color_set, 4);
    update_global_color(envName2, color_set);
  }
  else
  {
    rt_kprintf("解析颜色字符串失败！\n");
  }
}

void setTripleColorToEnv(const char *colors, const char *envName1, const char *envName2, const char *envName3)
{
  rt_kprintf("输入的颜色字符串: %s\n", colors);

  char color_get[7], color_get_2[7], color_get_3[7]; // 存储解析出的颜色
  int parsedItems = sscanf(colors, "[#%6s, #%6s, #%6s]", color_get, color_get_2, color_get_3);

  if (parsedItems == 3)
  {
    uint32_t color_set = strtol(color_get, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName1);
    ef_set_env_blob(envName1, &color_set, 4);
    update_global_color(envName1, color_set);

    color_set = strtol(color_get_2, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName2);
    ef_set_env_blob(envName2, &color_set, 4);
    update_global_color(envName2, color_set);

    color_set = strtol(color_get_3, NULL, 16);
    rt_kprintf("提取颜色 0x%06X 完成，保存到 %s\n", color_set, envName3);
    ef_set_env_blob(envName3, &color_set, 4);
    update_global_color(envName3, color_set);
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
    setColorToEnv(message->mode.colors, "chest_color");
    break;
  case 2:
    setColorToEnv(message->mode.colors, "basic_color");
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
  bool T = true, F = false;
  switch ((int)message->gnMode.id)
  {

  case 1:
  {
    is_double = false;
    is_center = false;
    setColorToEnv(message->gnMode.colors, "circle_color_1");
    break;
  }
  case 2:
  {
    is_double = true;
    is_center = false;
    setDualColorToEnv(message->gnMode.colors, "circle_color_1", "circle_color_2");
    break;
  }
  case 3:
  {
    is_double = false;
    is_center = true;
    setDualColorToEnv(message->gnMode.colors, "circle_color_1", "center_color");
    break;
  }
  case 4:
  {
    is_double = true;
    is_center = true;
    setTripleColorToEnv(message->gnMode.colors, "circle_color_1", "circle_color_2", "center_color");
    break;
  }
  default:
    break;
  }
}
#define CHUNK_SIZE 20  // 每次发送的最大字节数

void sendDataChunked(UART_HandleTypeDef *huart, const char *data) {
    size_t len = strlen(data);
    size_t sent = 0;

    while (sent < len) {
        size_t chunk_len = (len - sent > CHUNK_SIZE) ? CHUNK_SIZE : (len - sent);
        HAL_UART_Transmit(huart, (uint8_t *)(data + sent), chunk_len, HAL_MAX_DELAY);
        sent += chunk_len;
        rt_thread_mdelay(100);
    }
    HAL_UART_Transmit(huart, (uint8_t *)"\n", 1, HAL_MAX_DELAY);
}

void sendJsonData(cJSON *root) {
    char *json_str = cJSON_PrintUnformatted(root);
    if (json_str) {
        sendDataChunked(&huart3, json_str);
        sendDataChunked(&huart2, json_str);
        free(json_str);  // 释放 cJSON 分配的内存
    }
}


void commandParsingTask(void *prmt)
{
  while (1)
  {
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
        cJSON_AddNumberToObject(root, "mode_id", mode_id);
        cJSON_AddNumberToObject(root, "gn_mode_id", gn_mode_id);
        const char *envs[] = {
            "basic_color",
            "chest_color",
            "body_color",
            "wavy_color_1",
            "wavy_color_2",
            "circle_color_1",
            "circle_color_2",
            "center_color",
            "wings_color",
            "bird_color"};
        parseModeJSON(root, envs, 10);
        // Convert the JSON object to a string
        // Send the JSON string via USART
        sendJsonData(root);
        // Free the JSON string and object
        cJSON_Delete(root);
      }
      else if (parseMessage(usart3_rx_buffer, &message))
      {
        rt_kprintf("Mode Type: %d\n", message.modeType);
        rt_kprintf("Mode ID: %d, Colors: %s\n", message.mode.id, message.mode.colors);
        rt_kprintf("gnMode ID: %d, Colors: %s\n", message.gnMode.id, message.gnMode.colors);
        mode_id = message.mode.id;
        gn_mode_id = message.gnMode.id;
        gn_reset = true;
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
