#include "USER.h"

//#ifdef __vstdio
//class My_LIST
//{
//public:
//	int16_t			x;
//	int16_t			y;
//	int16_t			Index;
//	int16_t			Count;
//	const char			Name[8][16];
//	void creat(void);
//	void enter(void);
//	void init(void);
//private:
//};
//
//void USER_LIST_Enter_Callback(My_LIST *LIST);
//void USER_LIST_DisPlay_Callback(My_LIST *LIST);
//#endif

void My_LIST::creat(void)
{
	if (Count <= 0)
		return;
	if (Index < 0)
		Index = 0;
	if (Index >= Count)
		Index = Count - 1;

	USER_LIST_DisPlay_Callback((My_LIST *)&x);
}
void My_LIST::init(void)
{
	for (Count = 0; Count < 8; Count++)
	{
		if (Name[Count][0] == '\0')
			return;
	}
	Count = -1;
}
void My_LIST::enter(void)
{
	USER_LIST_Enter_Callback((My_LIST *)&x);
}

__weak void USER_LIST_Enter_Callback(My_LIST *LIST)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_ADD_Callback could be implemented in the user file
  */
}

__weak void USER_LIST_DisPlay_Callback(My_LIST *LIST)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_ADD_Callback could be implemented in the user file
  */
}
