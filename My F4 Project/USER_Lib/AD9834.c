#include "AD9834.h"

SPI_HandleTypeDef hspi3;

#define	SET_FSYNC()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_SET)
#define	CLR_FSYNC()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_6,GPIO_PIN_RESET)

#define	SET_RESET()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET)
#define	CLR_RESET()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET)

#define	SET_SLEEP()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET)
#define	CLR_SLEEP()		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET)

void AD9834_Init(void)
{
	{
		GPIO_InitTypeDef GPIO_InitStruct;

		/* GPIO Ports Clock Enable */
		__GPIOH_CLK_ENABLE();
		__GPIOC_CLK_ENABLE();
		__GPIOA_CLK_ENABLE();

		/*Configure GPIO pins : PC6 PC8 PC9 */
		GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_8|GPIO_PIN_9;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
		HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
	{
		hspi3.Instance = SPI3;
		hspi3.Init.Mode = SPI_MODE_MASTER;
		hspi3.Init.Direction = SPI_DIRECTION_2LINES;
		hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
		hspi3.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
		hspi3.Init.NSS = SPI_NSS_SOFT;
		hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
		hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
		hspi3.Init.TIMode = SPI_TIMODE_DISABLED;
		hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
		hspi3.Init.CRCPolynomial = 10;
		HAL_SPI_Init(&hspi3);
	}
	
	SET_FSYNC();
	SET_SLEEP();
	SET_RESET();

//	HAL_Delay(1);
//	CLR_RESET();
//	
	AD9834_Write(0x0100);
	AD9834_Write(0x2100);
	
	AD9834_Write(0x4000);
	AD9834_Write(0x4000);
	
	AD9834_Write(0x8000);
	AD9834_Write(0x8000);
	
	AD9834_Write(0xc000);
	AD9834_Write(0xe000);
	
	CLR_RESET();
	
	AD9834_Write(0x0c00);

}

void AD9834_Write(uint16_t data)
{
	CLR_FSYNC();
	HAL_SPI_Transmit(&hspi3,(uint8_t *)&data,1,10);
	SET_FSYNC();
}

void AD9834_SetFreq(uint32_t freq)
{
	uint32_t data;
	data = (uint32_t)(freq/(double)MCO_Freq*268435456);
	
	AD9834_Write(0x2100);
	AD9834_Write(0x8000|(data&0x3fff));
	AD9834_Write(0x8000|((data>>14)&0x3fff));
	AD9834_Write(0x0c00);
}
