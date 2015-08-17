#include "BSP.h"

//void BSP_OSC_Init(void);
//void BSP_LED_Init(void);
//void BSP_ADC_Init(void);
//void BSP_ADC_ex_Init(void);
//void BSP_DAC_Init(void);
//void BSP_MPU_Init(void);
//void BSP_LCD_Init(void);

SPI_HandleTypeDef hspi1;
TIM_HandleTypeDef htim1;
DMA_HandleTypeDef hdma_spi1_rx;
DMA_HandleTypeDef hdma_tim1_up;



ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;
DMA_HandleTypeDef hdma_adc1;

DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;
TIM_HandleTypeDef htim6;

TIM_HandleTypeDef htim3;

/*-------------------------------------------------------
功能	: 初始化
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_Init(void)
{
	BSP_OSC_Init();
	
	#if (configUSE_LED == 1)
	BSP_LED_Init();
	#endif
	#if (configUSE_DAC == 1)
	BSP_DAC_Init();
	#endif
	#if (configUSE_ADC123 == 1)
	BSP_ADC_Init();
	#endif
	#if (configUSE_ExternADC == 1)
	BSP_ADC_ex_Init();
	#endif
	#if (configUSE_LCD == 1)
	BSP_LCD_Init();
	#endif
	#if (configUSE_CODEC == 1)
	BSP_CODEC_Init();
	#endif
	#if (configUSE_PWM == 1)
	BSP_TIM3_Init();
	#endif
	#if (configUSE_RTT == 1)
	BSP_RTT_Init();
	#endif
	#if (configUSE_USB == 1)
	BSP_USB_Init();
	#endif
	#if (configUSE_MPU == 1)
	BSP_MPU_Init();
	#endif
}
/*-------------------------------------------------------
功能	: 初始化系统时钟,HSE=8MHz,HCLK=144MHz,APB1=36MHz,APB2=72MHz,USB=48MHz,ADCCLK=36MHz
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_OSC_Init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

	__PWR_CLK_ENABLE();

	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 288;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 6;
	HAL_RCC_OscConfig(&RCC_OscInitStruct);

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
							  |RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);

	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 256;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
	
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	
	#if (configUSE_MCO1 == 1)
	HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLLCLK, RCC_MCODIV_2);
	#endif
	#if (configUSE_MCO2 == 1)
	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_2);	//SYSCLK/2
	#endif
}

void BSP_RTT_Init(void)
{
	//SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
	SEGGER_RTT_Init();
}

/*-------------------------------------------------------
功能	: 初始化
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	configUSE_LED_Port_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = configUSE_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(configUSE_LED_Port, &GPIO_InitStruct);
}
/*-------------------------------------------------------
功能	: 初始化
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_LED_ON(void)
{
	#if (configUSE_LED == 1)
	HAL_GPIO_WritePin(configUSE_LED_Port, configUSE_LED_Pin, GPIO_PIN_RESET);
	#endif
}
/*-------------------------------------------------------
功能	: 初始化
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_LED_OFF(void)
{
	#if (configUSE_LED == 1)
	HAL_GPIO_WritePin(configUSE_LED_Port, configUSE_LED_Pin, GPIO_PIN_SET);
	#endif
}

/* TIM3 init function */
void BSP_TIM3_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_OC_InitTypeDef sConfigOC;

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 5-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim3.Init.Period = 144*250-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_PWM_Init(&htim3);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 10000;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);

  sConfigOC.Pulse = 10000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);

  sConfigOC.Pulse = 10000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3);

  sConfigOC.Pulse = 10000;
  HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_4);
}

