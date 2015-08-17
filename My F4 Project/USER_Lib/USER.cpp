#include "BSP.h"
#include "USER.h"

#include "FreqCount.h"

My_LIST LIST1 = { 152, 16, 0, 0, { { "1) MainPage    " }, { "2) Freq Count  " }, { "3) Delta time  " }, { "4) Duty cycle  " }, { "5) Setting     " } } };
My_LIST LIST2 = { 152, 16, 0, 0, { { "1) Page1       " }, { "2) Page2       " }, { "3) Page3       " }, { "4) Page4       " }, { "5) Page5       " } } };
My_MENU MENU1 = { 0, 0, { { "1) MainPage    " }, { "2) Encoder     " }, { "3) Fun1        " }, { "4) Fun2        " }, { "5) Fun3        " } } };

My_ENC ENC1 = { "ENC1", GPIOB, GPIO_PIN_12, GPIOB, GPIO_PIN_13, 0, 0 };
My_ENC ENC2 = { "ENC2", GPIOB, GPIO_PIN_14, GPIOB, GPIO_PIN_15, 0, 0 };

My_BTN BTN[] =
{
	{ GPIOD, GPIO_PIN_8, 0 },
	{ GPIOB, GPIO_PIN_11, 0 },
	{ GPIOB, GPIO_PIN_10, 0 },
	{ GPIOB, GPIO_PIN_1, 0 },
	{ GPIOD, GPIO_PIN_9, 0 },
	{ GPIOC, GPIO_PIN_5, 0 },
	{ GPIOB, GPIO_PIN_0, 0 },
};

extern uint32_t FreqCount;
extern uint32_t TimeCount;
extern uint32_t DelayCount;
extern uint32_t CycleCount;
extern uint32_t AFreqCount;

extern uint16_t TIM2Mode;
extern uint16_t TIM5Mode;
extern uint16_t TIM8Mode;

extern double Freq;
extern double Time;
extern double Delay;
extern double Cycle;

#define	MainPage	0
#define	Encoder		1
#define	Fun1		2
#define	Fun2		3
#define	Fun3		4

#define	BTN_LIGHT	0
#define	BTN_CENTER	1
#define	BTN_UP		2
#define	BTN_RIGHT	3
#define	BTN_DOWN	4
#define	BTN_Enter	5
#define	BTN_SYSTEM	6


#define IsMENU(MenuIndex)		if(MenuIndex == MENU1.Index)
#define IsENC(EncIndex)			if(uEnc == &EncIndex)
#define IsBTN(BTNIndex)			if(uBtn == &BTN[BTNIndex])

#define Count2Uint		


void LCD_Show_Freq(void)
{
	int8_t uint = 0;
	double num = Freq;
	
	if(num < 0)
		return;
	
	if(Freq > 128000000)
	{
		BSP_LCD_ShowString2(96+24,12,"     N/A  Hz");
		return;
	}
	
	for(;num > 900;)
	{
		num = num / 1000.0;
		uint ++;
	}
	
	if(uint == 0)
		BSP_LCD_Printf2(96+24,12,"%8.4f %s",num," Hz");
	if(uint == 1)
		BSP_LCD_Printf2(96+24,12,"%8.4f %s",num,"kHz");
	if(uint == 2)
		BSP_LCD_Printf2(96+24,12,"%8.4f %s",num,"MHz");
}

void LCD_Show_Time(void)
{
	int8_t uint = 0;
	double num = Time;
	
	if(num < 0)
		return;
	
	if(num < 7.8125)
	{
		BSP_LCD_ShowString2(96-24,12,"     N/A  ns");
		return;
	}
	
	for(;num > 900;)
	{
		num = num / 1000.0;
		uint ++;
	}
	
	if(uint == 0)
		BSP_LCD_Printf2(96-24,12,"%8.4f %s",num," ns");
	if(uint == 1)
		BSP_LCD_Printf2(96-24,12,"%8.4f %s",num," us");
	if(uint == 2)
		BSP_LCD_Printf2(96-24,12,"%8.4f %s",num," ms");
}

void LCD_Show_Delay(void)
{
	int8_t uint = 0;
	double num = Delay;
	
	if(num < 0)
		return;
	
	for(;num > 900;)
	{
		num = num / 1000.0;
		uint ++;
	}
	
	if(uint == 0)
		BSP_LCD_Printf2(96,12,"%8.4f %s",num," ns");
	if(uint == 1)
		BSP_LCD_Printf2(96,12,"%8.4f %s",num," us");
	if(uint == 2)
		BSP_LCD_Printf2(96,12,"%8.4f %s",num," ms");
}

void LCD_Show_Cycle(void)
{
	double num = Cycle*100;
	
	BSP_LCD_Printf2(96,12,"%8.1f %s",num," % ");
	BSP_LCD_Printf(32,108,"FreqCount  = %8d",AFreqCount);
	BSP_LCD_Printf(32-16,108,"CycleCount = %8.3f %",Cycle);
}

void USER_ENC_ADD_Callback(My_ENC *uEnc)
{
	IsENC(ENC2)
	{
		IsMENU(MainPage)
			LIST1.Index++;
		IsMENU(Encoder)
			//LIST2.Index++;
		{
			TIM5Mode++;
			if(TIM5Mode>=3)
				TIM5Mode=2;
			
			extern TIM_HandleTypeDef htim5;
			HAL_TIM_Base_DeInit(&htim5);
			Mode_Select();
		}
		IsMENU(Fun3)
		{
			TIM5Mode++;
			if(TIM5Mode>=3)
				TIM5Mode=2;
			
			extern TIM_HandleTypeDef htim5;
			HAL_TIM_Base_DeInit(&htim5);
			Mode_Select();
		}
	}
}

void USER_ENC_CUT_Callback(My_ENC *uEnc)
{
	IsENC(ENC2)
	{
		IsMENU(MainPage)
			LIST1.Index--;
		IsMENU(Encoder)
			//LIST2.Index--;
		{
			TIM5Mode--;
			if(TIM5Mode>=3)
				TIM5Mode=0;
			extern TIM_HandleTypeDef htim5;
			HAL_TIM_Base_DeInit(&htim5);
			Mode_Select();
		}
		IsMENU(Fun3)
		{
			TIM5Mode--;
			if(TIM5Mode>=3)
				TIM5Mode=0;
			
			extern TIM_HandleTypeDef htim5;
			HAL_TIM_Base_DeInit(&htim5);
			Mode_Select();
		}
	}
}


void USER_BTN_DOWN_Callback(My_BTN *uBtn)
{
//	SEGGER_RTT_printf(0, "%d is down\r\n", (*uBtn).GPIO_PIN);
//	SEGGER_RTT_printf(0, "%s is down\r\n", (*uBtn).Name); 
	IsBTN(BTN_SYSTEM)
		MENU1.Index = 0;
	
	IsMENU(MainPage)
	{
		IsBTN(BTN_Enter)
			LIST1.enter();
		IsBTN(BTN_CENTER)
			LIST1.enter();
		IsBTN(BTN_UP)
			LIST1.Index--;
		IsBTN(BTN_DOWN)
			LIST1.Index++;
	}
	IsMENU(Encoder)
	{
		IsBTN(BTN_UP)
			LIST2.Index--;
		IsBTN(BTN_DOWN)
			LIST2.Index++;
	}
	IsMENU(Fun1)
	{
		
	}
	IsMENU(Fun2)
	{
		
	}
	IsMENU(Fun3)
	{
		
	}
}

void USER_BTN_HOLD_Callback(My_BTN *uBtn)
{
	IsBTN(BTN_SYSTEM)
		HAL_NVIC_SystemReset();
	
	IsMENU(MainPage)
	{
		IsBTN(BTN_LIGHT)
		{
			
		}
		IsBTN(BTN_CENTER)
		{
			
		}
		IsBTN(BTN_UP)
		{
			
		}
		IsBTN(BTN_RIGHT)
		{
			
		}
		IsBTN(BTN_DOWN)
		{
			
		}
		IsBTN(BTN_Enter)
		{
			
		}
		IsBTN(BTN_SYSTEM)
		{
			
		}
	}
	IsMENU(Encoder)
	{
		
	}
	IsMENU(Fun1)
	{
		
	}
	IsMENU(Fun2)
	{
		
	}
	IsMENU(Fun3)
	{
		
	}
}

void USER_LIST_Enter_Callback(My_LIST *LIST)
{
	IsMENU(MainPage)
		MENU1.Index = LIST1.Index;
}

void USER_MENU_DisPlay_Callback(My_MENU * Menu)
{	
	IsMENU(MainPage)
	{
		
//		BSP_LCD_Printf(100 - 25, 200, "%10d", FreqCount);
		BSP_LCD_ShowString(180+32, 16, "Press \"Enter\"  to Function Page\r\nPress \"System\" to Reset System.\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\nBuilding 2015/08/15 19:29:00\0");
		LIST1.creat();
	}
	IsMENU(Encoder)
	{
		//BSP_LCD_ShowString2(96+24,12,"1.000000  Hz");
		LCD_Show_Freq();
		LCD_Show_Time();
		//BSP_LCD_Printf(180,12,"Mode=%d",TIM5Mode);
		//BSP_LCD_ShowString2(96-24,12,"1.000000  ns");
		//LIST2.creat();
	}
	IsMENU(Fun1)
	{
		//BSP_LCD_Demo();
		//BSP_LCD_Printf(100 - 75, 271, "%s", "FUN1");
		//BSP_LCD_ShowString2(96,12,"1.000000  ns");
		LCD_Show_Delay();
	}
	IsMENU(Fun2)
	{
		//BSP_LCD_Clear();
		//BSP_LCD_Printf(100 - 75, 271, "%s", "FUN2");
		//BSP_LCD_ShowString2(96,12,"    50.00  %");
		LCD_Show_Cycle();
	}
	IsMENU(Fun3)
	{
		//BSP_LCD_Clear();
		BSP_LCD_SetColor(BLACK, BLUE);
		BSP_LCD_Printf(200, 0, "%s", "               Setting                ");
		BSP_LCD_SetColor(WHITE, BLACK);
		BSP_LCD_Printf(180,12,"Mode=%d",TIM5Mode);
		BSP_LCD_Printf(100 - 75, 271, "%5d", ENC1.Count);
		BSP_LCD_Printf(100 - 50, 271, "%5d", ENC2.Count);
	}
}

void USER_MENU_Change_Callback(My_MENU * Menu)
{
	BSP_LCD_Clear();
	IsMENU(MainPage)
		Mode_DeInit();
	IsMENU(Encoder)
		SysMode = 0;
	IsMENU(Fun1)
		SysMode = 1;
	IsMENU(Fun2)
		SysMode = 2;
	Mode_Select();
}

void USER_LIST_DisPlay_Callback(My_LIST *LIST)
{
	uint16_t i;
	
	for (i = 0; i < (*LIST).Count; i++)
	{
		if (i == (*LIST).Index)
			BSP_LCD_SetColor(BLACK, WHITE);
		BSP_LCD_ShowString((*LIST).x - i * 16, (*LIST).y, (*LIST).Name[i]);
		BSP_LCD_SetColor(BLUE, BLACK);
	}
}
