#include "USER.h"

void My_BTN::read(void)
{
	if(!(HAL_GPIO_ReadPin(GPIOx,GPIO_PIN)))
		IsEnter ++;
	else if(IsEnter != 0)
	{
		USER_BTN_UP_Callback((My_BTN *)&GPIOx);
		IsEnter = 0;
	}
	if(IsEnter == 5)		//first click,filter = 5 clk 
	{
		USER_BTN_DOWN_Callback((My_BTN *)&GPIOx);
	}
	if(IsEnter == 200)	//hold on
	{
		USER_BTN_HOLD_Callback((My_BTN *)&GPIOx);
	}
	if(IsEnter > 1000)
		IsEnter = 1000;
}

void My_BTN::init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

  __GPIOA_CLK_ENABLE();
  __GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
  __GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

__weak void USER_BTN_UP_Callback(My_BTN *uBtn)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_BTN_UP_Callback could be implemented in the user file
  */
}
__weak void USER_BTN_DOWN_Callback(My_BTN *uBtn)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_BTN_DOWN_Callback could be implemented in the user file
  */
}
__weak void USER_BTN_HOLD_Callback(My_BTN *uBtn)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_BTN_HOLD_Callback could be implemented in the user file
  */
}

