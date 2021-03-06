
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"

/* USER CODE BEGIN Includes */
#include "string.h"
#include "lwip/api.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

osThreadId defaultTaskHandle;
osMessageQId DMA_Ethernet_QueueHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
volatile uint16_t ADC_Data[128]; //2*1KB
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_DMA_Init(void);

static void MX_ADC1_Init(void);

static void MX_USART2_UART_Init(void);

static void MX_TIM3_Init(void);

void StartDefaultTask(void const *argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
#define printf(format) sprintf (pbuffer, format);HAL_UART_Transmit(&huart2, pbuffer, strlen(pbuffer), 0xFFFF)
#define aprintf(format, ...) sprintf (pbuffer, format, __VA_ARGS__);HAL_UART_Transmit(&huart2, pbuffer, strlen(pbuffer), 0xFFFF)
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
osThreadId netTaskHandle;
void StartNetTask(void const *argument);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

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
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_USART2_UART_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */

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

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 512);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */

	/* USER CODE END RTOS_THREADS */

	/* Create the queue(s) */
	/* definition and creation of DMA_Ethernet_Queue */
/* what about the sizeof here??? cd native code */
	osMessageQDef(DMA_Ethernet_Queue, 64, uint16_t);
	DMA_Ethernet_QueueHandle = osMessageCreate(osMessageQ(DMA_Ethernet_Queue), NULL);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */


	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

	}
	/* USER CODE END 3 */

}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
	RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
	RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
								  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);

	/**Configure the Systick
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/**Configure the Systick interrupt time
	*/
	__HAL_RCC_PLLI2S_ENABLE();

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* ADC1 init function */
static void MX_ADC1_Init(void) {

	ADC_ChannelConfTypeDef sConfig;

	/**Common config
	*/
	hadc1.Instance = ADC1;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T3_TRGO;
	hadc1.Init.DataAlign = ADC_DATAALIGN_LEFT;
	hadc1.Init.NbrOfConversion = 1;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure Regular Channel
	*/
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* TIM3 init function */
static void MX_TIM3_Init(void) {

	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;

	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 1124; //72000k/(1124+1)=64k per sec
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 4; //16k per sec
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/* USART2 init function */
static void MX_USART2_UART_Init(void) {

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK) {
		_Error_Handler(__FILE__, __LINE__);
	}

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) {
	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void) {

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : LED_Pin */
	GPIO_InitStruct.Pin = LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
	for (int i = 0; i < 64; ++i) {
		osMessagePut(DMA_Ethernet_QueueHandle, ADC_Data[i], 0);
	}
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	for (int i = 64; i < 128; ++i) {
		osMessagePut(DMA_Ethernet_QueueHandle, ADC_Data[i], 0);
	}

}
/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const *argument) {
	/* init code for LWIP */
	MX_LWIP_Init();

	/* USER CODE BEGIN 5 */
	char *pbuffer = pvPortMalloc(200);
	struct neconn *nc;
	struct neconn *in_nc;
	struct netbuf *nb;
	volatile err_t res;
	uint16_t len;
	ip_addr_t local_ip;
	ip_addr_t remote_ip;
	ip4addr_aton("10.1.1.2", &local_ip);
	ip4addr_aton("10.1.1.1", &remote_ip);
	char *buffer = pvPortMalloc(2048);


	/*//MCU as client WORKS:!!!
	 nc = netconn_new(NETCONN_TCP);
	if (nc == NULL) {
		printf("new error\r\n");
		while (1) osDelay(1);
	}

	res = netconn_bind(nc, &local_ip, 0);
	if (res != 0) {
		aprintf("bind error: %d\r\n", res);
		while (1) osDelay(1);
	}

	res = netconn_connect(nc, &remote_ip, 3000);
	if (res != 0) {
		aprintf("connect error: %d\r\n", res);
		while (1) osDelay(1);
	}

	sprintf(buffer, "\r\n");
	res = netconn_write(nc, buffer, strlen(buffer), NETCONN_COPY);
	if (res != 0) {
		aprintf("write error: %d\r\n", res);
		while (1) osDelay(1);
	}

	res = netconn_recv(nc, &nb);
	if (res != 0) {
		aprintf("recv error: %d\r\n", res);
		while (1) osDelay(1);
	}

	len = netbuf_len(nb);
	netbuf_copy(nb, buffer, len);
	netbuf_delete(nb);
	buffer[len] = 0;
	aprintf("Received %d bytes:\r\n%s\r\n", len, buffer);
	netconn_close(nc);
	netconn_delete(nc);
//sprintf writes into self
	printf("Client sequence completed successful!\r\n");
	 */
//As server works!!))

//	aprintf("%d", SystemCoreClock);
	nc = netconn_new(NETCONN_TCP);
	if (nc == NULL) {
		aprintf("new error: %d\r\n", res);
		while (1) osDelay(1);
	}


	res = netconn_bind(nc, IP_ADDR_ANY, 3081);
	if (res != 0) {
		aprintf("bind error: %d\r\n", res);
		while (1) osDelay(1);
	}


	res = netconn_listen(nc);
	if (res != 0) {
		aprintf("listen error: %d\r\n", res);
		while (1) osDelay(1);
	}
	//Infinite loop
	for (;;) {
		res = netconn_accept(nc, &in_nc);
		if (res != 0) {
			aprintf("listen error: %d\r\n", res);
		} else {
			osThreadDef(netTask, StartNetTask, osPriorityNormal, 0, 256);
			netTaskHandle = osThreadCreate(osThread(netTask), (void *) in_nc);
		}
//		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
//		osDelay(300);
//		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
//		osDelay(300);
	}
	/* USER CODE END 5 */
}

/* StartNetTask function */
void StartNetTask(void const *argument) {
	/* USER CODE BEGIN StartNetTask */
	struct netconn *nc = (struct netconn *) argument;
	struct netbuf *nb;
	char *buffer = pvPortMalloc(2048);
	char *pbuffer = pvPortMalloc(200);
	uint16_t len;
//	printf("Incoming connection\r\n");
	HAL_TIM_Base_Start_IT(&htim3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t *) &ADC_Data, 128);
//	int ad = DMA1_Channel1->CCR;
//	int ad2 = ADC1->CR2;

	osEvent event;
	int nul=0;
	for (;;) {
		event = osMessageGet(DMA_Ethernet_QueueHandle, 0);
//		sprintf(buffer, "Event %x!\r\n", event.status);
//		netconn_write(nc, buffer, strlen(buffer), NETCONN_COPY);
		if (event.status == osEventMessage) {
			netconn_write(nc, &nul, event.value.p, NETCONN_COPY); //1KB
		} else{
			HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
		}
	}
	/* USER CODE END StartNetTask */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while (1) {
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
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
