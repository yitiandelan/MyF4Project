#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"

void Mode_Select(void);
void Mode_Get(void);
void Mode_Init(void);
void Mode_DeInit(void);
extern uint16_t SysMode;
extern uint16_t TIM2Mode;
extern uint16_t TIM5Mode;
extern uint16_t TIM8Mode;
	 
#ifdef __cplusplus
 }
#endif



