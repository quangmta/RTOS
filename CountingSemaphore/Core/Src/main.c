/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
//#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "stdlib.h"
#include "string.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;

//osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
//void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// create task defines

TaskHandle_t HPThandler;
void HPT_TASK(void *pvParameters);

TaskHandle_t MPThandler;
void MPT_TASK(void *pvParameters);

TaskHandle_t LPThandler;
void LPT_TASK(void *pvParameters);

TaskHandle_t VLPThandler;
void VLPT_TASK(void *pvParameters);

// semaphore related
SemaphoreHandle_t CountingSem;

// resource related
int resource[3]={111,222,333};
int indx = 0;

//uart related
uint8_t rx_data = 0;

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
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_UART_Receive_IT(&huart2, &rx_data, 1);

  CountingSem = xSemaphoreCreateCounting(3,0);
  if (CountingSem == NULL) HAL_UART_Transmit(&huart2,(uint8_t *)"Unable to Create Semaphore\n\n",28,100);
  else HAL_UART_Transmit(&huart2,(uint8_t *)"Couting Semaphore created successfully\n\n",41,1000);



  //create TASKS

  xTaskCreate(HPT_TASK, "HPT", 128, NULL, 3, &HPThandler);
  xTaskCreate(MPT_TASK, "MPT", 128, NULL, 2, &MPThandler);
  xTaskCreate(LPT_TASK, "LPT", 128, NULL, 1, &LPThandler);
  xTaskCreate(VLPT_TASK, "VLPT", 128, NULL, 0, &VLPThandler);

  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  //osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  //defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  //osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	HAL_UART_Receive_IT(&huart2,&rx_data, 1);
	if(rx_data=='r')
	{
		//release semaphore
		//the xHigherPriorityTaskWoken parameter must be initialized to pdFALSE as
		//it will get set to pdTRUE inside the interrupt safe API function if a
		// context switch is required
		BaseType_t xHigherPriorityTaskToken = pdFALSE;
		xSemaphoreGiveFromISR(CountingSem,&xHigherPriorityTaskToken);//ISR SAFE VERSION
		xSemaphoreGiveFromISR(CountingSem,&xHigherPriorityTaskToken);//ISR SAFE VERSION
		xSemaphoreGiveFromISR(CountingSem,&xHigherPriorityTaskToken);//ISR SAFE VERSION

		/* Pass the xHigherPriorityTaskWoken value into portEND_SWITCHING_ISR(). If
		 xHigherPriorityTaskWoken was set to pdTRUE inside xSemaphoreGiveFromISR()
		 then calling portEND_SWITCHING_ISR() will request a context switch. If
		 xHigherPriorityTaskWoken is still pdFALSE then calling
		 portEND_SWITCHING_ISR() will have no effect */

		portEND_SWITCHING_ISR(xHigherPriorityTaskToken);
	}

}


void HPT_TASK(void *pvParameters)
{
	char sresource[3];
	int semcount = 0;
	char ssemcount[2];

	//Give 3 semaphores at the beginning...
	xSemaphoreGive(CountingSem);
	xSemaphoreGive(CountingSem);
	xSemaphoreGive(CountingSem);

	while(1)
	{
		char str[150];
		strcpy(str,"Entered HPT Task\n About to ACQUIRE the Semaphore\n\n");
		semcount = uxSemaphoreGetCount(CountingSem);
		itoa (semcount, ssemcount, 10);
		strcat (str, "Tokens available are: ");
		strcat (str, ssemcount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		xSemaphoreTake(CountingSem,portMAX_DELAY);

		itoa(resource[indx],sresource,10);
		strcpy(str,"Leaving HPT Task\n Data ACCEDDED is:: ");
		strcat(str,sresource);
		strcat(str,"\n Not releasing the Semaphore \n\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		indx++;
		if(indx>2) indx=0;
		vTaskDelay(3000);
		//vTaskDelete(NULL);
	}

}

void MPT_TASK(void *pvParameters)
{
	char sresource[3];
	int semcount = 0;
	char ssemcount[2];

	//Give 3 semaphores at the beginning...
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);

	while(1)
	{
		char str[150];
		strcpy(str,"Entered MPT Task\n About to ACQUIRE the Semaphore\n\n");
		semcount = uxSemaphoreGetCount(CountingSem);
		itoa (semcount, ssemcount, 10);
		strcat (str, "Tokens available are: ");
		strcat (str, ssemcount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		xSemaphoreTake(CountingSem,portMAX_DELAY);

		itoa(resource[indx],sresource,10);
		strcpy(str,"Leaving MPT Task\n Data ACCEDDED is:: ");
		strcat(str,sresource);
		strcat(str,"\n Not releasing the Semaphore \n\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		indx++;
		if(indx>2) indx=0;
		vTaskDelay(2000);
//		vTaskDelete(NULL);
	}

}
void LPT_TASK(void *pvParameters)
{
	char sresource[3];
	int semcount = 0;
	char ssemcount[2];

	//Give 3 semaphores at the beginning...
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);

	while(1)
	{
		char str[150];
		strcpy(str,"Entered LPT Task\n About to ACQUIRE the Semaphore\n\n");
		semcount = uxSemaphoreGetCount(CountingSem);
		itoa (semcount, ssemcount, 10);
		strcat (str, "Tokens available are: ");
		strcat (str, ssemcount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		xSemaphoreTake(CountingSem,portMAX_DELAY);

		itoa(resource[indx],sresource,10);
		strcpy(str,"Leaving LPT Task\n Data ACCEDDED is:: ");
		strcat(str,sresource);
		strcat(str,"\n Not releasing the Semaphore \n\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		indx++;
		if(indx>2) indx=0;
		vTaskDelay(1000);
//		vTaskDelete(NULL);
	}
}

void VLPT_TASK(void *pvParameters)
{
	char sresource[3];
	int semcount = 0;
	char ssemcount[2];

	//Give 3 semaphores at the beginning...
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);
//	xSemaphoreGive(CountingSem);

	while(1)
	{
		char str[150];
		strcpy(str,"Entered VLPT Task\n About to ACQUIRE the Semaphore\n\n");
		semcount = uxSemaphoreGetCount(CountingSem);
		itoa (semcount, ssemcount, 10);
		strcat (str, "Tokens available are: ");
		strcat (str, ssemcount);
		strcat (str, "\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		xSemaphoreTake(CountingSem,portMAX_DELAY);

		itoa(resource[indx],sresource,10);
		strcpy(str,"Leaving VLPT Task\n Data ACCEDDED is:: ");
		strcat(str,sresource);
		strcat(str,"\n Not releasing the Semaphore \n\n\n");
		HAL_UART_Transmit(&huart2,(uint8_t *)str,strlen(str),HAL_MAX_DELAY);

		indx++;
		if(indx>2) indx=0;
		vTaskDelay(500);
	}

}



/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
//void StartDefaultTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
//  /* Infinite loop */
//  for(;;)
//  {
//    osDelay(1);
//  }
//  /* USER CODE END 5 */
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
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

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
