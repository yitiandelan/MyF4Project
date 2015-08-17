#ifdef __cplusplus
 extern "C" {
#endif
	 
#include "stm32f4xx_hal.h"
//#include "string.h"


typedef struct
{
	const char		Name[8];
	GPIO_TypeDef* GPIOx;
	uint16_t 			GPIO_PIN;
	uint16_t			IsEnter;
} BTN_TypeDef;


#ifdef __cplusplus

class My_BTN
{
public:
//	const char		Name[8];
	GPIO_TypeDef*	GPIOx;
	uint16_t		GPIO_PIN;
	uint16_t		IsEnter;
//BTN_TypeDef * BTNx;
	void read(void);
	void init(void);
private:
};

void USER_BTN_UP_Callback(My_BTN *uBtn);
void USER_BTN_DOWN_Callback(My_BTN *uBtn);
void USER_BTN_HOLD_Callback(My_BTN *uBtn);

class My_ENC
{
public:
	const char		Name[8];
	GPIO_TypeDef*	DM_GPIOx;
	uint16_t		DM_GPIO_PIN;
	GPIO_TypeDef*	DP_GPIOx;
	uint16_t		DP_GPIO_PIN;
	uint16_t		IsEnter;
	int32_t			Count;
	void read(void);
	void init(void);
private:
};

void USER_ENC_ADD_Callback(My_ENC *uEnc);
void USER_ENC_CUT_Callback(My_ENC *uEnc);


class My_MENU
{
public:
	int16_t			Index;
	int16_t			Count;
	const char			Name[8][16];
	void creat(void);
	void init(void);
private:
};

void USER_MENU_DisPlay_Callback(My_MENU * Menu);
void USER_MENU_Change_Callback(My_MENU * Menu);

class My_LIST
{
public:
	int16_t			x;
	int16_t			y;
	int16_t			Index;
	int16_t			Count;
	const char			Name[8][16];
	void creat(void);
	void enter(void);
	void init(void);
private:
};

void USER_LIST_Enter_Callback(My_LIST *LIST);
void USER_LIST_DisPlay_Callback(My_LIST *LIST);
#endif


#ifdef __cplusplus
}
#endif
