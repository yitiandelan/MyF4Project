#include "USER.h"

void My_MENU::creat(void)
{
	static int16_t lastindex = -1;
	
	if (Count <= 0)
		return;
	if (Index < 0)
		Index = 0;
	if (Index >= Count)
		Index = Count - 1;
	
	if(lastindex == Index)
	{
		USER_MENU_DisPlay_Callback((My_MENU *)&Index);
	}
	else
	{
		USER_MENU_Change_Callback((My_MENU *)&Index);
		lastindex = Index;
	}
	
}

void My_MENU::init(void)
{
	for (Count = 0; Count < 8; Count++)
	{
		if (Name[Count][0] == '\0')
			return;
	}
	Count = -1;
}



//MENU_TypeDef MENU[] = 
//{
//	{"MAIN",0},
//	{"ENCR",1},
//	{"FUN1",2},
//	{"FUN2",3},
//	{"FUN3",4},
//};




//uint16_t MENU_Count;
//uint16_t MENU_SelectIndex;
//uint16_t MENU_DefaultIndex = 0;


//void USER_MENU_Init(void)
//{
//	MENU_Count = sizeof(MENU)/sizeof(MENU[0]);
//	MENU_SelectIndex = 0;
////	USER_MENU_Change_Callback(&MENU[MENU_SelectIndex]);
//}

//void USER_MENU_Fun(void)
//{
//	static int16_t LastSelectIndex = -1;
//	if(LastSelectIndex == MENU_SelectIndex)
//	{
//		USER_MENU_Function_Callback(&MENU[MENU_SelectIndex]);
//		USER_MENU_DisPlay_Callback(&MENU[MENU_SelectIndex]);
//	}
//	else
//	{
//		USER_MENU_Change_Callback(&MENU[MENU_SelectIndex]);
//		LastSelectIndex = MENU_SelectIndex;
//	}
//}

//void USER_MENU_Select(char * MenuName)
//{
//	uint16_t i;
//	for(i=0;i<MENU_Count;i++)
//	{
//		if(strcmp(MenuName,(char *)MENU[i].Name) == 0)
//		{
//			MENU_SelectIndex = i;
//			//USER_MENU_Change_Callback(&MENU[MENU_SelectIndex]);
//			return;
//		}
//	}
//	if(strcmp(MenuName,"CHANGE") == 0)
//	{
//		MENU_SelectIndex++;
//		if(MENU_SelectIndex == MENU_Count)
//			MENU_SelectIndex = MENU_DefaultIndex;
//	}
//}

//__weak void USER_MENU_Function_Callback(MENU_TypeDef * Menu)
//{
//	/* NOTE : This function Should not be modified, when the callback is needed,
//            the USER_ENC_ADD_Callback could be implemented in the user file
//  */
//}
__weak void USER_MENU_DisPlay_Callback(My_MENU * Menu)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_ADD_Callback could be implemented in the user file
  */
}
__weak void USER_MENU_Change_Callback(My_MENU * Menu)
{
	/* NOTE : This function Should not be modified, when the callback is needed,
            the USER_ENC_ADD_Callback could be implemented in the user file
  */
}

