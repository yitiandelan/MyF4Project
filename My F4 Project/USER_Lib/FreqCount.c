#include "FreqCount.h"
#include "BSP.h"
//#include "USER.h"

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;
DMA_HandleTypeDef hdma_tim2_ch1;
DMA_HandleTypeDef hdma_tim5_ch1;

uint32_t FreqCount;
uint32_t TimeCount;
uint32_t DelayCount;
uint32_t CycleCount;
uint32_t AFreqCount;
uint32_t ACycleCount;

double Freq = 0;
double Time = 0;
double Delay = 0;
double Cycle = 0;
double ACycle = 0;

double ACycle1 = 0;
double ACycle2 = 0;
double ACycle3 = 0;

uint32_t lastCCR1[2];
uint32_t lastCCR2[2];


uint16_t SysMode = 0;

uint16_t TIM2Mode = 0;
uint16_t TIM5Mode = 0;
uint16_t TIM8Mode = 0;

void TIM2_CH1_COUNT_Enable(void);
void TIM2_CH1_GATED_Enable(void);
void TIM5_CH1_COUNT_Enable(void);
void TIM5_CH2_COUNT_Enable(void);
void TIM3_CH1_COUNT_Enable(void);

void TIM8_CH1_GATED_Enable(void);
void TIM8_CH2_GATED_Enable(void);

void TIM2_CH1_IC_Enable(void);
	
void MX_TIM7_Init(void);
void TIM5_IC_Enable(void);

#define TIM7_1S()		TIM7->ARR = 10000-1
#define TIM7_250uS()	TIM7->ARR = 2500-1
#define TIM7_500uS()	TIM7->ARR = 5000-1

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	Mode_Get();
	BSP_LED_ON();
	BSP_LED_OFF();
}
void Mode_Select(void)
{
	if(SysMode == 0)
	{
		if(TIM5Mode == 0)
			TIM5_CH1_COUNT_Enable();	//B频率1 PA0
		if(TIM5Mode == 1)
			TIM5_CH2_COUNT_Enable();	//B频率2 PA1
		if(TIM5Mode == 2)
			TIM5_IC_Enable();			//B频率3 PA0 测周模式
	}
	if(SysMode == 1)
	{
		TIM2_CH1_COUNT_Enable();		//A频率1 PA5 计数模式
		if(TIM8Mode == 0)
			TIM8_CH1_GATED_Enable();	//A&B占空比1 PC6
		if(TIM8Mode == 1)
			TIM8_CH2_GATED_Enable();	//A&B占空比2 PC7		
	}
	if(SysMode == 2)
	{
		TIM2_CH1_GATED_Enable();		//A占空比PA5 门控模式
		TIM3_CH1_COUNT_Enable();		//A频率3 PA6 计数模式
	}
}
void Mode_Init(void)
{
	__GPIOH_CLK_ENABLE();
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();
	__GPIOF_CLK_ENABLE();
	
	MX_TIM7_Init();
	HAL_TIM_Base_Start_IT(&htim7);	//1s时基
	
	Mode_Select();
}

void Mode_DeInit(void)
{
	SysMode = 3;
//	HAL_TIM_Base_Stop(&htim2);
	HAL_TIM_Base_DeInit(&htim2);
//	HAL_TIM_Base_Stop(&htim3);
	HAL_TIM_Base_DeInit(&htim3);
//	HAL_TIM_Base_Stop(&htim5);
	HAL_TIM_Base_DeInit(&htim5);
//	HAL_TIM_Base_Stop(&htim8);
	HAL_TIM_Base_DeInit(&htim8);
}

void CH2GetFreq(void)
{
	if(TIM5Mode == 0)		//tim5 PA0 计数模式测B通道频率
	{
		FreqCount = TIM5->CNT;		
		TIM5->CNT = 0;
		
		if(FreqCount != 0)
		{
			Freq = FreqCount;
			Time = 1000000000/Freq;
		}
		else
		{
			Freq = 0;
			Time = 0;
		}
	}
	if(TIM5Mode == 1)		//tim5 PA1 计数模式测B通道频率
	{
		FreqCount = 4*(TIM5->CNT);		
		TIM5->CNT = 0;
		
		if(FreqCount != 0)
		{
			Freq = FreqCount;
			Time = 1000000000/Freq;
		}
		else
		{
			Freq = 0;
			Time = 0;
		}
	}
	if(TIM5Mode == 2)		//tim5 PA0 输入捕获模式测B通道频率
	{
		if(lastCCR1[0] > lastCCR1[1])
			FreqCount = lastCCR1[0] - lastCCR1[1];
		else
			FreqCount = lastCCR1[1] - lastCCR1[0];
		
		Time = FreqCount/0.072;
		Freq = 1000000000/Time;
	}
}

void CH1GetFreq(void)
{
	AFreqCount = (TIM3->CNT)*200;	//tim3 PA6 计数模式测A通道频率
	TIM3->CNT = 0;
	
	if(SysMode == 1)
	{
		AFreqCount = TIM2->CNT;
		TIM2->CNT = 0;
	}
}

void CH1GetCycle(void)
{
	CycleCount = TIM2->CNT;
	TIM2->CNT = 0;   
	
	if(CycleCount>72000000)
	CycleCount = 72000000;
	Cycle = CycleCount/72000000.0;
		
	if(AFreqCount > 50000)			//频率大于50k 开启校正
	{
		if(AFreqCount != 0)
		{
			double t = 1.0/(double)AFreqCount;
			double p = t * Cycle + 0.0000000045;
			
			Cycle = p/t;
		}
		else
			Cycle = 1;
	}
}

void CH3GetCycle(void)
{
	ACycleCount = TIM8->CNT;			//tim8 PC6 PC7 测A&B 占空比
	TIM8->CNT = 0;
	
	ACycle = ACycleCount/10000.0;
	
//	if(AFreqCount != 0)
//		Delay = 1000000000/(double)AFreqCount*ACycle +12.53;
}

void Mode_Get(void)
{
	static uint16_t count = 0;
	
	if(SysMode == 0)
	{
		TIM7_1S();
		
		CH2GetFreq();
	}
	if(SysMode == 1)
	{
		if(AFreqCount >= 25)
			TIM7_250uS();
		else
			TIM7_1S();
		
		count++;
		if(count == 4)
		{
			CH1GetFreq();
			count = 0;
			
//		if(AFreqCount<100)
//		{
//		ACycle2 -=0.1;
//		ACycle3 -=0.1;
//		}LMV
			if(AFreqCount >= 25)
			{
				ACycle = (0.5-( ACycle2-ACycle1));
			}
			else
			{
				ACycle = (0.5-( ACycle2/4.0-ACycle1/4.0))*4;
			}
				
			
			//ACycle3 =(0.5-( ACycle2-ACycle1));
			
			if(AFreqCount != 0)
				Delay = 1000000000*ACycle/(double)AFreqCount + 20 - 2.49;
			else
				Delay = 0;
		}
		
		if(count == 0)
		{
			CH3GetCycle();
			TIM8_CH1_GATED_Enable();
			ACycle1 = ACycle;
		}
		if(count == 1)
			TIM8->CNT = 0;
		if(count == 2)
		{
			CH3GetCycle();
			TIM8_CH2_GATED_Enable();
			ACycle2 = ACycle;
		}
		if(count == 3)
			TIM8->CNT = 0;
		
	}
	if(SysMode == 2)
	{
		TIM7_1S();
		
		CH1GetFreq();
		CH1GetCycle();
	}
}

void MX_TIM7_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig;

  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 7200-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 10000-1;
  HAL_TIM_Base_Init(&htim7);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig);

}

void MX_TIM5_CH1_COUNT_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4000000000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim5, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);
}

void MX_TIM5_CH2_COUNT_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4000000000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim5, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);

}

void MX_TIM5_IC_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_MasterConfigTypeDef sMasterConfig;
  TIM_IC_InitTypeDef sConfigIC;

  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4000000000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim5);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig);

  HAL_TIM_IC_Init(&htim5);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig);

  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0x0f;
  HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_1);

//  HAL_TIM_IC_ConfigChannel(&htim5, &sConfigIC, TIM_CHANNEL_2);

}

void MX_TIM2_CH1_COUNT_Init(void)
{

  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4000000000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim2, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

void MX_TIM2_CH1_IC_Init(void)
{
	TIM_ClockConfigTypeDef sClockSourceConfig;
	TIM_MasterConfigTypeDef sMasterConfig;
	TIM_IC_InitTypeDef sConfigIC;

	__DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4000000000;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	HAL_TIM_Base_Init(&htim2);

	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

	HAL_TIM_IC_Init(&htim2);

	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

	sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
	sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
	sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
	sConfigIC.ICFilter = 0;
	HAL_TIM_IC_ConfigChannel(&htim2, &sConfigIC, TIM_CHANNEL_1);
}

void MX_TIM2_CH1_GATED_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4000000000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim2);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim2, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig);

}

void MX_TIM3_CH1_COUNT_Init(void)
{
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 200-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 60000;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  HAL_TIM_Base_Init(&htim3);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_EXTERNAL1;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim3, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig);
}

void MX_TIM8_CH1_GATED_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 3600-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 60000;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim8);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim8, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);

}

void MX_TIM8_CH2_GATED_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig;
  TIM_SlaveConfigTypeDef sSlaveConfig;
  TIM_MasterConfigTypeDef sMasterConfig;

  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 3600-1;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 60000;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&htim8);

  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig);

  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
  sSlaveConfig.InputTrigger = TIM_TS_TI2FP2;
  sSlaveConfig.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;
  sSlaveConfig.TriggerFilter = 0;
  HAL_TIM_SlaveConfigSynchronization(&htim8, &sSlaveConfig);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig);

}

void TIM2_CH1_IC_Enable(void)
{
//	HAL_TIM_IC_Stop(&htim2,TIM_CHANNEL_1);
//	HAL_TIM_IC_DeInit(&htim2);
//	
	MX_TIM2_CH1_IC_Init();
	//HAL_TIM_IC_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_IC_Start_DMA(&htim2,TIM_CHANNEL_1,(uint32_t *)lastCCR2,2);
}
void TIM8_CH1_GATED_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim8);
//	HAL_TIM_Base_DeInit(&htim8);
	
	MX_TIM8_CH1_GATED_Init();
	HAL_TIM_Base_Start(&htim8);
}

void TIM8_CH2_GATED_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim8);
//	HAL_TIM_Base_DeInit(&htim8);
//	
	MX_TIM8_CH2_GATED_Init();
	HAL_TIM_Base_Start(&htim8);
}

void TIM5_CH1_COUNT_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim5);
//	HAL_TIM_Base_DeInit(&htim5);
//	
	MX_TIM5_CH1_COUNT_Init();
	HAL_TIM_Base_Start(&htim5);
}

void TIM5_CH2_COUNT_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim5);
//	HAL_TIM_Base_DeInit(&htim5);
//	
	MX_TIM5_CH2_COUNT_Init();
	HAL_TIM_Base_Start(&htim5);
}

void TIM5_IC_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim5);
//	HAL_TIM_Base_DeInit(&htim5);
//	
	__DMA1_CLK_ENABLE();
	MX_TIM5_IC_Init();
	HAL_TIM_IC_Start_DMA(&htim5,TIM_CHANNEL_1,(uint32_t *)lastCCR1,2);
//	HAL_TIM_Base_Start(&htim5);
}

void TIM2_CH1_COUNT_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim2);
//	HAL_TIM_Base_DeInit(&htim2);
//	
	MX_TIM2_CH1_COUNT_Init();
	HAL_TIM_Base_Start(&htim2);
}

void TIM2_CH1_GATED_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim2);
//	HAL_TIM_Base_DeInit(&htim2);
//	
	MX_TIM2_CH1_GATED_Init();
	HAL_TIM_Base_Start(&htim2);
}

void TIM3_CH1_COUNT_Enable(void)
{
//	HAL_TIM_Base_Stop(&htim3);
//	HAL_TIM_Base_DeInit(&htim3);
//	
	MX_TIM3_CH1_COUNT_Init();
	HAL_TIM_Base_Start(&htim3);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	GPIO_InitTypeDef GPIO_InitStruct;
  if(htim_base->Instance==TIM6)
  {

    __TIM6_CLK_ENABLE();
  }
  if(htim_base->Instance==TIM2)
  {
    __TIM2_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	hdma_tim2_ch1.Instance = DMA1_Stream5;
    hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;
    hdma_tim2_ch1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim2_ch1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_tim2_ch1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim2_ch1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim2_ch1);

    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC1],hdma_tim2_ch1);
  }
  if(htim_base->Instance==TIM3)
  {
    __TIM3_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
  if(htim_base->Instance==TIM5)
  {
    __TIM5_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	  
	hdma_tim5_ch1.Instance = DMA1_Stream2;
    hdma_tim5_ch1.Init.Channel = DMA_CHANNEL_6;
    hdma_tim5_ch1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_tim5_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_tim5_ch1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_tim5_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_tim5_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_tim5_ch1.Init.Mode = DMA_CIRCULAR;
    hdma_tim5_ch1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_tim5_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    hdma_tim5_ch1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_tim5_ch1.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma_tim5_ch1.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma_tim5_ch1);

    __HAL_LINKDMA(htim_base,hdma[TIM_DMA_ID_CC1],hdma_tim5_ch1);

  }
  if(htim_base->Instance==TIM7)
  {
    __TIM7_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  }
  if(htim_base->Instance==TIM8)
  {
    __TIM8_CLK_ENABLE();
  
    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
  }
}
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance==TIM6)
	{

		__TIM6_CLK_DISABLE();

	}
	if(htim_base->Instance==TIM2)
	{
		__TIM2_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5);
	}
	if(htim_base->Instance==TIM3)
	{
		__TIM3_CLK_DISABLE();
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_6);
	}
	if(htim_base->Instance==TIM5)
	{
		__TIM5_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
		HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);
		
		HAL_DMA_DeInit(htim_base->hdma[TIM_DMA_ID_CC1]);
	}
	  if(htim_base->Instance==TIM7)
	  {
		__TIM7_CLK_DISABLE();

		HAL_NVIC_DisableIRQ(TIM7_IRQn);
	  }
	if(htim_base->Instance==TIM8)
	{
		__TIM8_CLK_DISABLE();

		HAL_GPIO_DeInit(GPIOC, GPIO_PIN_6|GPIO_PIN_7);
	}
}
