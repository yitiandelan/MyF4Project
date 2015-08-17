#include "USER.h"

#define ENC_A_SET(ENCx)		(HAL_GPIO_ReadPin(DM_GPIOx,DM_GPIO_PIN))
#define ENC_B_SET(ENCx)		(HAL_GPIO_ReadPin(DP_GPIOx,DP_GPIO_PIN))

void My_ENC::read(void)
{
	if(ENC_A_SET(Enc)&ENC_B_SET(Enc))		//nothing
	{
		IsEnter = 0;
	}
	else
	{
		IsEnter ++;
	}
	if(IsEnter == 1)			//first click，filter = 1 clk
	{
		if(ENC_A_SET(Enc))
		{
			Count --;
			USER_ENC_CUT_Callback((My_ENC *)&Name);
		}
		if(ENC_B_SET(Enc))
		{
			Count ++;
			USER_ENC_ADD_Callback((My_ENC *)&Name);
		}
	}
	if(IsEnter > 1000)
	{
		IsEnter = 1000;
	}
}

void My_ENC::init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();

	GPIO_InitStruct.Pin = DM_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DM_GPIOx, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DP_GPIO_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(DP_GPIOx, &GPIO_InitStruct);
}


__weak void USER_ENC_ADD_Callback(My_ENC *uEnc)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_ADD_Callback could be implemented in the user file
  */
}
__weak void USER_ENC_CUT_Callback(My_ENC *uEnc)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_CUT_Callback could be implemented in the user file
  */
}
