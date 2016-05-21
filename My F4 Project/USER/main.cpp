#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "BSP.h"

osThreadId SYSTEMHandle;
osThreadId DISPLAYHandle;
osThreadId INPUTHandle;
osThreadId DSPHandle;

void SystemTask(void const * argument);
void DispalyTask(void const * argument);
void InputTask(void const * argument);
void DSPTask(void const * argument);

int main(void)
{
	HAL_Init();
	BSP_Init();
	
	BSP_LED_OFF();

	/* definition and creation of SYSTEM */
	osThreadDef(SYSTEM, SystemTask, osPriorityHigh, 0, 128);
	SYSTEMHandle = osThreadCreate(osThread(SYSTEM), NULL);

	/* definition and creation of DISPLAY */
	osThreadDef(DISPLAY, DispalyTask, osPriorityLow, 0, 128);
	DISPLAYHandle = osThreadCreate(osThread(DISPLAY), NULL);

	/* definition and creation of INPUT */
	osThreadDef(INPUT, InputTask, osPriorityNormal, 0, 128);
	INPUTHandle = osThreadCreate(osThread(INPUT), NULL);

	/* definition and creation of DSP */
	osThreadDef(DSP, DSPTask, osPriorityHigh, 0, 128);
	DSPHandle = osThreadCreate(osThread(DSP), NULL);
	
	osKernelStart(NULL, NULL);
	
	for (;;)
	{
		BSP_LED_ON();
	}
}

/* SystemTask function */
void SystemTask(void const * argument)
{
	SEGGER_RTT_printf(0, "STM32F407VGT6 SN=%8x%8x%8x\r\n", *(__IO uint32_t*)(0x1fff7a10), *(__IO uint32_t*)(0x1fff7a14), *(__IO uint32_t*)(0x1fff7a18));
	for(;;)
	{
		osDelay(1000);
	}
}

/* DispalyTask function */
void DispalyTask(void const * argument)
{
	for(;;)
	{
		osDelay(50);
	}
}

/* InputTask function */
void InputTask(void const * argument)
{
	for(;;)
	{
		osDelay(5);
	}
}

/* DSPTask function */
void DSPTask(void const * argument)
{
	for(;;)
	{
		osDelay(1000);
	}
}

