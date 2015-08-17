#include "stm32f4xx_hal.h"

#define MCO_Freq	72000000

extern SPI_HandleTypeDef hspi3;

void AD9834_Init(void);
void AD9834_Write(uint16_t data);

void AD9834_SetFreq(uint32_t freq);


