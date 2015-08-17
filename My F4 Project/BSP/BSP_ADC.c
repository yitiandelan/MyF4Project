#include "BSP.h"

int16_t SPI1TXBuffer[20];
//int16_t SPI1RXBuffer[ADCBufSize];

#define ADCBufSize	2000

int16_t  ADC0Buf[ADCBufSize];
uint16_t ADC123Buf[ADCBufSize*3];


/*-------------------------------------------------------
功能	: 初始化ADC123
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_ADC_Init(void)
{
	__GPIOA_CLK_ENABLE();
	__DMA2_CLK_ENABLE();
	
	{
		ADC_MultiModeTypeDef multimode;

		/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
		*/
		hadc1.Instance = ADC1;
		hadc1.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		hadc1.Init.Resolution = ADC_RESOLUTION12b;
		hadc1.Init.ScanConvMode = DISABLE;
		hadc1.Init.ContinuousConvMode = DISABLE;
		hadc1.Init.DiscontinuousConvMode = DISABLE;
		hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
		hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC3;
		hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc1.Init.NbrOfConversion = 1;
		hadc1.Init.DMAContinuousRequests = ENABLE;
		hadc1.Init.EOCSelection = EOC_SEQ_CONV;
		HAL_ADC_Init(&hadc1);

		/**Configure the ADC multi-mode 
		*/
		multimode.DMAAccessMode = ADC_DMAACCESSMODE_1;
		HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode);
	}
	{
		ADC_MultiModeTypeDef multimode;
		ADC_ChannelConfTypeDef sConfig;

		/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
		*/
		hadc2.Instance = ADC2;
		hadc2.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		hadc2.Init.Resolution = ADC_RESOLUTION12b;
		hadc2.Init.ScanConvMode = DISABLE;
		hadc2.Init.ContinuousConvMode = DISABLE;
		hadc2.Init.DiscontinuousConvMode = DISABLE;
		hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc2.Init.NbrOfConversion = 1;
		hadc2.Init.DMAContinuousRequests = DISABLE;
		hadc2.Init.EOCSelection = EOC_SINGLE_CONV;
		HAL_ADC_Init(&hadc2);

		/**Configure the ADC multi-mode 
		*/
		multimode.Mode = ADC_TRIPLEMODE_REGSIMULT;
		multimode.DMAAccessMode = ADC_DMAACCESSMODE_1;
		multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
		HAL_ADCEx_MultiModeConfigChannel(&hadc2, &multimode);

		/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
		*/
		sConfig.Channel = ADC_CHANNEL_1;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		HAL_ADC_ConfigChannel(&hadc2, &sConfig);
	}
	{
		ADC_MultiModeTypeDef multimode;
		ADC_ChannelConfTypeDef sConfig;

		/**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
		*/
		hadc3.Instance = ADC3;
		hadc3.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
		hadc3.Init.Resolution = ADC_RESOLUTION12b;
		hadc3.Init.ScanConvMode = DISABLE;
		hadc3.Init.ContinuousConvMode = DISABLE;
		hadc3.Init.DiscontinuousConvMode = DISABLE;
		hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
		hadc3.Init.NbrOfConversion = 1;
		hadc3.Init.DMAContinuousRequests = DISABLE;
		hadc3.Init.EOCSelection = EOC_SINGLE_CONV;
		HAL_ADC_Init(&hadc3);

		/**Configure the ADC multi-mode 
		*/
		multimode.Mode = ADC_TRIPLEMODE_REGSIMULT;
		multimode.DMAAccessMode = ADC_DMAACCESSMODE_1;
		multimode.TwoSamplingDelay = ADC_TWOSAMPLINGDELAY_5CYCLES;
		HAL_ADCEx_MultiModeConfigChannel(&hadc3, &multimode);

		/**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
		*/
		sConfig.Channel = ADC_CHANNEL_2;
		sConfig.Rank = 1;
		sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
		HAL_ADC_ConfigChannel(&hadc3, &sConfig);
	}
	
	HAL_ADCEx_MultiModeStart_DMA(&hadc1,(uint32_t*)ADC123Buf,ADCBufSize*3);
	__HAL_ADC_ENABLE(&hadc1);
	__HAL_ADC_ENABLE(&hadc2);
	__HAL_ADC_ENABLE(&hadc3);
	
}
/*-------------------------------------------------------
功能	: 初始化外部ADC
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_ADC_ex_Init(void)
{
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;
	TIM_OC_InitTypeDef sConfigOC;
	
	// PE2 --> MPU_CS
	// user code begin @{
	
	__GPIOB_CLK_ENABLE();
	
	__DMA2_CLK_ENABLE();
	
	{
		GPIO_InitTypeDef  GPIO_InitStruct;
	__GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);			//MPU9250	CS
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);			//MS5611	CS
	//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);		//ADC			CS
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);
	}

	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 1-1;
	htim1.Init.CounterMode = TIM_COUNTERMODE_DOWN;
	htim1.Init.Period = 72*1-1;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	HAL_TIM_PWM_Init(&htim1);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig);

	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig);

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 34;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);
	
	// user code end   @}
	
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
	HAL_SPI_Init(&hspi1);
	

	__HAL_SPI_ENABLE(&hspi1);
	
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	
	hspi1.Instance->CR2 |= SPI_CR2_TXDMAEN;
	
	HAL_DMA_Start(&hdma_tim1_up ,(uint32_t)SPI1TXBuffer, (uint32_t)(SPI1_BASE+0x0c), 20);
	
	__HAL_TIM_ENABLE_DMA(&htim1,TIM_DMA_UPDATE);
	
	HAL_SPI_Receive_DMA(&hspi1, (uint8_t *)ADC0Buf, ADCBufSize);
}
/*-------------------------------------------------------
功能	: 返回对应ADC值
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
//int16_t BSP_GetADCNum(uint8_t ADCx)
//{
//	int16_t ADCNum,i;
//	
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_13, GPIO_PIN_RESET);
//	
//	for(i=80;i!=0;i--);
//	
//	HAL_SPI_TransmitReceive(&hspi1,(uint8_t *)SPI1TXBuffer,(uint8_t *)SPI1RXBuffer,2,5000);
//			
//	ADCNum = SPI1RXBuffer[0]<<8 | SPI1RXBuffer[1];
//	
//	return ADCNum;
//}
