#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "SEGGER_RTT.h"
#include "usb_device.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

#include "BSP_LCD.h"
#include "BSP_MPU.h"

#define configUSE_MCO1				0
#define configUSE_MCO2				0

#define configUSE_LED				1

#define configUSE_ADC123			0
#define configUSE_ExternADC			0
#define configUSE_DAC				1
#define configUSE_DDS				0
#define configUSE_CODEC				0

#define configUSE_RTT				1
#define configUSE_USB				1
#define configUSE_LCD				1

#define configUSE_PWM				0
#define configUSE_MPU				0

#define configUSE_LED_Port				GPIOA
#define configUSE_LED_Port_CLK_ENABLE	__GPIOA_CLK_ENABLE
#define configUSE_LED_Pin				GPIO_PIN_3

#define _MAX(a,b)	(a>b?a:b)
#define _MIN(a,b)	(a>b?b:a)

extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_tim1_up;

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc1;

extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_dac1;
extern TIM_HandleTypeDef htim6;

extern TIM_HandleTypeDef htim3;

extern USBD_HandleTypeDef hUsbDeviceFS;

extern SRAM_HandleTypeDef hsram1;

extern int16_t SPI1TXBuffer[];
//extern int16_t SPI1RXBuffer[];

extern int16_t  ADC0Buf[];
extern uint16_t ADC123Buf[];
extern uint32_t DAC12Buf[];

extern int16_t I2S_ADCBuf[];
extern int16_t I2S_DACBuf[];


void BSP_Init(void);

void BSP_OSC_Init(void);
void BSP_LED_Init(void);
void BSP_ADC_Init(void);
void BSP_ADC_ex_Init(void);
void BSP_DAC_Init(void);


void BSP_TIM3_Init(void);
void BSP_RTT_Init(void);
void BSP_USB_Init(void);
void BSP_CODEC_Init(void);

void BSP_CODEC_Start(void);


void BSP_LED_ON(void);
void BSP_LED_OFF(void);

#ifdef __cplusplus
}
#endif
