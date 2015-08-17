#include "stm32f4xx_hal.h"

#define RED	  0XFF0000
#define GREEN 0x00FF00
#define BLUE  0X0000FF
#define BRED  0XFF00FF
#define GRED  0XFFFF00
#define GBLUE 0X00FFFF
#define BLACK 0X000000
#define WHITE 0XFFFFFF

void BSP_LCD_Init(void);

void LCD_SetWindos(uint16_t xsta, uint16_t xend,uint16_t ysta,uint16_t yend);
void BSP_LCD_SetColor(uint32_t PointColor,uint32_t BackColor);

void BSP_LCD_PUSH(uint32_t);
void BSP_LCD_Clear(void);

void BSP_LCD_DrawPiont(uint16_t x,uint16_t y);
void BSP_LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void BSP_LCD_DrawBlock(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void BSP_LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void BSP_LCD_DrawCircle(uint16_t x0,uint16_t y0,uint16_t r);

void BSP_LCD_ResetPiont(uint16_t x,uint16_t y);
void BSP_LCD_ResetLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void BSP_LCD_ResetBlock(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
void BSP_LCD_ResetRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void BSP_LCD_ResetCircle(uint16_t x0,uint16_t y0,uint16_t r);

void BSP_LCD_Demo(void);
void BSP_LCD_ShowBMP(void);
void BSP_LCD_ShowChar(uint16_t x,uint16_t y,unsigned char c);
void BSP_LCD_ShowString(uint16_t x,uint16_t y,const char * c);

void BSP_LCD_Printf(uint16_t x,uint16_t y,const char * format,...);

void BSP_LCD_ShowChar2(uint16_t x,uint16_t y,unsigned char c);
void BSP_LCD_ShowString2(uint16_t x,uint16_t y,const char * c);
void BSP_LCD_Printf2(uint16_t x,uint16_t y,const char * format,...);
