#include "BSP.h"
#include "font.h"
#include "BSP_LCD_BMP.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

//#define ShowBMP
//#define AlphaMode

#define Bank1_LCD_D    ((uint32_t)0x60010000)		//disp Data ADDR
#define Bank1_LCD_C    ((uint32_t)0x60000000)		//disp Reg ADDR

#define LCD_Write_Cmd(a)	*(__IO uint8_t *) (Bank1_LCD_C) = a
#define LCD_Write_Data(a)	*(__IO uint8_t *) (Bank1_LCD_D) = a

SRAM_HandleTypeDef hsram1;

void LCDReadId(void);

uint8_t LCD_ID[4];
char str[64];
uint8_t p_r,p_g,p_b,b_r,b_g,b_b;

/*-------------------------------------------------------
功能	: 初始化LCD
输入	: void
返回	: void
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	FSMC_NORSRAM_TimingTypeDef Timing;
	
	__GPIOD_CLK_ENABLE();
	__GPIOE_CLK_ENABLE();

	/*Configure GPIO pin : PD12 */
	GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /** Perform the SRAM1 memory initialization sequence
  */
  hsram1.Instance = FSMC_NORSRAM_DEVICE;
  hsram1.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;
  /* hsram1.Init */
  hsram1.Init.NSBank = FSMC_NORSRAM_BANK1;
  hsram1.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;
  hsram1.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;
  hsram1.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_8;
  hsram1.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;
  hsram1.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW;
  hsram1.Init.WrapMode = FSMC_WRAP_MODE_DISABLE;
  hsram1.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;
  hsram1.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;
  hsram1.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;
  hsram1.Init.ExtendedMode = FSMC_EXTENDED_MODE_DISABLE;
  hsram1.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;
  hsram1.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;
  /* Timing */
  Timing.AddressSetupTime = 2;
  Timing.AddressHoldTime = 0;
  Timing.DataSetupTime = 3;
  Timing.BusTurnAroundDuration = 0;
  Timing.CLKDivision = 0;
  Timing.DataLatency = 0;
  Timing.AccessMode = FSMC_ACCESS_MODE_A;
  /* ExtTiming */

  HAL_SRAM_Init(&hsram1, &Timing, NULL);
	
	
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
	HAL_Delay(2);
	LCD_Write_Cmd(0x11);
	HAL_Delay(2);
	
	LCDReadId();
	
//	LCD_Write_Cmd(0x11);         //Sleep Out,0x10=Sleep In.退出睡眠模式
	LCD_Write_Cmd(0x36);         //Memory Data Access Control 
	LCD_Write_Data(0x00);        //Memory Data Access Control Parameter?
	
	LCD_Write_Cmd(0x3a);         //Interface Pixel Format 
	LCD_Write_Data(0x07);        //16BIT=0X05,18BIT=0X06,24BIT=0X07 

//LCD_Write_Cmd(0x6d);         //Write Display Brightness
//LCD_Write_Data(0x13);        //Write Display Brightness Parameter,0x00-0xff.
//LCD_Write_Cmd(0x53);         //Write CTRL Display,
//LCD_Write_Data(0x2c);        //Memory Data Access Control Parameter?
	
	BSP_LCD_Clear();
	BSP_LCD_SetColor(WHITE,BLACK);
	
	LCD_Write_Cmd(0x29);
}

void LCDReadId(void)
{
	LCD_Write_Cmd(0xda);      
	LCD_ID[0]=*(__IO uint8_t *) (Bank1_LCD_D);
	LCD_ID[0]=*(__IO uint8_t *) (Bank1_LCD_D);
	LCD_Write_Cmd(0xdb); 
	LCD_ID[1]=*(__IO uint8_t *) (Bank1_LCD_D);
	LCD_ID[1]=*(__IO uint8_t *) (Bank1_LCD_D);
	LCD_Write_Cmd(0xdc); 
	LCD_ID[2]=*(__IO uint8_t *) (Bank1_LCD_D);
	LCD_ID[2]=*(__IO uint8_t *) (Bank1_LCD_D);
}

void LCD_SetWindos(uint16_t xsta, uint16_t xend,uint16_t ysta,uint16_t yend)
{
	LCD_Write_Cmd(0x2a);      //设置X坐标
	LCD_Write_Data((uint8_t)(xsta>>8));       //Y方向开始高8位
	LCD_Write_Data((uint8_t)(xsta&0x00ff));   //Y方向开始低8位
	LCD_Write_Data((uint8_t)(xend>>8));       //Y方向开始高8位
	LCD_Write_Data((uint8_t)(xend&0x00ff));   //Y方向开始低8位

	LCD_Write_Cmd(0x2b);      //设置Y坐标
	LCD_Write_Data((uint8_t)(ysta>>8));       //Y方向开始高8位
	LCD_Write_Data((uint8_t)(ysta&0x00ff));   //Y方向开始低8位
	LCD_Write_Data((uint8_t)(yend>>8));       //Y方向开始高8位
	LCD_Write_Data((uint8_t)(yend&0x00ff));   //Y方向开始低8位
}

void BSP_LCD_SetColor(uint32_t PointColor,uint32_t BackColor)
{
	p_r = (PointColor >> 16)&0xff;
	p_g = (PointColor >> 8 )&0xff;
	p_b = (PointColor >> 0 )&0xff;
	
	b_r = (BackColor >> 16)&0xff;
	b_g = (BackColor >> 8 )&0xff;
	b_b = (BackColor >> 0 )&0xff;
}

void BSP_LCD_Clear(void)
{
	uint32_t i;
	LCD_SetWindos(0,240-1,0,320-1);
	LCD_Write_Cmd(0x2c);           

	for(i=0;i<320*240;i++)  
	{
			LCD_Write_Data (0x00);    
			LCD_Write_Data (0x00);    
			LCD_Write_Data (0x00);
	}							  
}

void BSP_LCD_PUSH(uint32_t c)
{
	uint32_t i;
	uint8_t r,g,b;
	
	r = (c >> 16)&0xff;
	g = (c >> 8 )&0xff;
	b = (c >> 0 )&0xff;
	
	LCD_SetWindos(0,240-1,0,320-1);
	LCD_Write_Cmd(0x2c);           
	for(i=0;i<320*240;i++)   
	{
		  LCD_Write_Data (r);    
		  LCD_Write_Data (g);    
		  LCD_Write_Data (b);			  
	}
}

void BSP_LCD_ShowChar(uint16_t x,uint16_t y,unsigned char c)
{
	uint8_t i,j,temp;
	uint32_t k;
	
	if((x >= 240)|(y >= 320)|(c < ' '))
		return;
	
	LCD_SetWindos(x,x+16-1,y,y+8-1);
	LCD_Write_Cmd(0x2c); 
	
	c -= ' ';
	
	for(i=0;i<16;i++)
	{
		temp = font8x16[c][i];
		
		#ifdef AlphaMode
		if((i%2)==0)
			k=((y+i/2)*240+x)*3;
		#endif
		
		for(j=0;j<8;j++)
		{
			if(temp&0x01)
			{
				LCD_Write_Data(p_r);
				LCD_Write_Data(p_g);
				LCD_Write_Data(p_b);
			}
			else
			{
				#ifdef AlphaMode
					LCD_Write_Data(gImage_ok[k]);
					LCD_Write_Data(gImage_ok[k+1]);
					LCD_Write_Data(gImage_ok[k+2]);
				#else
					LCD_Write_Data(b_r);
					LCD_Write_Data(b_g);
					LCD_Write_Data(b_b);
				#endif				
			}
			temp >>= 1;
			k+=3;
		}
	}
}

void BSP_LCD_DrawPiont(uint16_t x,uint16_t y)
{
	if((x >= 240)|(y >= 320))
		return;
	
	LCD_SetWindos(x,x,y,y);
	LCD_Write_Cmd(0x2c); 
	
	LCD_Write_Data(p_r);
	LCD_Write_Data(p_g);
	LCD_Write_Data(p_b);
}

void BSP_LCD_DrawLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	uint16_t t; 
	int16_t xerr=0,yerr=0,delta_x,delta_y,distance; 
	int16_t incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		BSP_LCD_DrawPiont(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	} 
}

void BSP_LCD_DrawBlock(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	uint32_t i;
	
	if((x1>=240)|(x2>=240)|(y1>=320)|(y2>=320))
		return;
	
	LCD_SetWindos(x1,x2,y1,y2);
	LCD_Write_Cmd(0x2c); 
	
	for(i=0;i<(x2+1-x1)*(y2+1-y1);i++)
	{
		LCD_Write_Data(p_r);
		LCD_Write_Data(p_g);
		LCD_Write_Data(p_b);
	}
}

void BSP_LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	BSP_LCD_DrawLine(x1,y1,x2,y1);
	BSP_LCD_DrawLine(x1,y1,x1,y2);
	BSP_LCD_DrawLine(x1,y2,x2,y2);
	BSP_LCD_DrawLine(x2,y1,x2,y2);
}

void BSP_LCD_DrawCircle(uint16_t x0,uint16_t y0,uint16_t r)
{
	int16_t a,b;
	int16_t di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		BSP_LCD_DrawPiont(x0+a,y0-b);             //5
 		BSP_LCD_DrawPiont(x0+b,y0-a);             //0           
		BSP_LCD_DrawPiont(x0+b,y0+a);             //4               
		BSP_LCD_DrawPiont(x0+a,y0+b);             //6 
		BSP_LCD_DrawPiont(x0-a,y0+b);             //1       
 		BSP_LCD_DrawPiont(x0-b,y0+a);             
		BSP_LCD_DrawPiont(x0-a,y0-b);             //2             
  	BSP_LCD_DrawPiont(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}

void BSP_LCD_ShowString(uint16_t x,uint16_t y,const char * c)
{
	int16_t len = strlen(c);
	uint16_t y1 = y;
	
	if(len < 0)
		return;
	while(len--)
	{
		if((*c == '\r')&(*(c+1) == '\n'))
		{
			c += 2;
			x -= 16;
			y = y1;
		}
		else
		{
			BSP_LCD_ShowChar(x,y,*c);
			y += 8;
			c++;
		}
	}
}

void BSP_LCD_Printf(uint16_t x,uint16_t y,const char * format,...)
{
	int16_t len;
	char *p = str;
	
	va_list args;
	va_start(args,format);
	len = vsnprintf((char*)str,64,format,args);
	va_end(args);
	
	if(len<=0)
		return;
	if(len>=64)
		for(;;);	//str[].len = 64
	
	while(len--)
	{
		BSP_LCD_ShowChar(x,y,*p);
		y += 8;
		p++;
	}
}

void BSP_LCD_ShowChar2(uint16_t x,uint16_t y,unsigned char c)
{
	uint8_t i,j,temp;
	uint32_t k;
	
	if((x >= 240)|(y >= 320)|(c < ' '))
		return;
	
	LCD_SetWindos(x,x+48-1,y,y+24-1);
	LCD_Write_Cmd(0x2c); 
	
	if((c < ' ')|(c > '}'))
		return;
	c -= ' ';

	
	for(i=0;i<144;i++)
	{
		temp = font24x48[c][i];
		
		#ifdef AlphaMode
		if((i%2)==0)
			k=((y+i/2)*240+x)*3;
		#endif
		
		for(j=0;j<8;j++)
		{
			if(temp&0x01)
			{
				LCD_Write_Data(p_r);
				LCD_Write_Data(p_g);
				LCD_Write_Data(p_b);
			}
			else
			{
				#ifdef AlphaMode
					LCD_Write_Data(gImage_ok[k]);
					LCD_Write_Data(gImage_ok[k+1]);
					LCD_Write_Data(gImage_ok[k+2]);
				#else
					LCD_Write_Data(b_r);
					LCD_Write_Data(b_g);
					LCD_Write_Data(b_b);
				#endif				
			}
			temp >>= 1;
			k+=3;
		}
	}
}

void BSP_LCD_ShowString2(uint16_t x,uint16_t y,const char * c)
{
	int16_t len = strlen(c);
	uint16_t y1 = y;
	
	if(len < 0)
		return;
	while(len--)
	{
		if((*c == '\r')&(*(c+1) == '\n'))
		{
			c += 2;
			x -= 48;
			y = y1;
		}
		else
		{
			BSP_LCD_ShowChar2(x,y,*c);
			y += 24;
			c++;
		}
	}
}

void BSP_LCD_Printf2(uint16_t x,uint16_t y,const char * format,...)
{
	int16_t len;
	char *p = str;
	
	va_list args;
	va_start(args,format);
	len = vsnprintf((char*)str,64,format,args);
	va_end(args);
	
	if(len<=0)
		return;
	if(len>=64)
		for(;;);	//str[].len = 64
	
	while(len--)
	{
		BSP_LCD_ShowChar2(x,y,*p);
		y += 24;
		p++;
	}
}

void BSP_LCD_Demo(void)
{
	static uint16_t hsv = 0;
	uint8_t r,g,b;

	switch(hsv/60)
	{
		case 0:
			r = 255;
			b = 0;
			g = (hsv%60*255)/60;
			break;
		case 1:
			g = 255;
			b = 0;
			r = ((60-hsv%60)*255)/60;
			break;
		case 2:
			g = 255;
			r = 0;
			b = (hsv%60*255)/60;
			break;
		case 3:
			b = 255;
			r = 0;
			g = ((60-hsv%60)*255)/60;
			break;
		case 4:
			b = 255;
			g = 0;
			r = (hsv%60*255)/60;
			break;
		case 5:
			r = 255;
			g = 0;
			b = ((60-hsv%60)*255)/60;
			break;
	}
	
	hsv+=1;
	if(hsv == 360)
		hsv = 0;
	BSP_LCD_PUSH((r<<16)|(g<<8)|b);
}

void BSP_LCD_ShowBMP(void)
{
	#ifdef ShowBMP
	unsigned long num;
	LCD_SetWindos(0,240-1,0,320-1);
	LCD_Write_Cmd(0x2c); 
	for(num=0;num<320*240*3;num++)
	{		
		LCD_Write_Data(gImage_ok[num]);
	}
	#endif
}

#ifdef AlphaMode
void BSP_LCD_ResetPiont(uint16_t x,uint16_t y)
{
	uint32_t k;
	
	if((x >= 240)|(y >= 320))
		return;
	
	k=(y*240+x)*3;
	
	LCD_SetWindos(x,x,y,y);
	LCD_Write_Cmd(0x2c); 
	
	LCD_Write_Data(gImage_ok[k]);
	LCD_Write_Data(gImage_ok[k+1]);
	LCD_Write_Data(gImage_ok[k+2]);
}

void BSP_LCD_ResetLine(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	uint16_t t; 
	int16_t xerr=0,yerr=0,delta_x,delta_y,distance; 
	int16_t incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		BSP_LCD_ResetPiont(uRow,uCol);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	} 
}

void BSP_LCD_ResetBlock(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2)
{
	uint32_t i,j,k;
	
	if((x1>=240)|(x2>=240)|(y1>=320)|(y2>=320))
		return;
	
	LCD_SetWindos(x1,x2,y1,y2);
	LCD_Write_Cmd(0x2c); 
	
	for(i=0;i<(y2+1-y1);i++)
	{
		k=((y1+i)*240+x1)*3;
		for(j=0;j<(x2+1-x1);j++)
		{
			LCD_Write_Data(gImage_ok[k]);
			LCD_Write_Data(gImage_ok[k+1]);
			LCD_Write_Data(gImage_ok[k+2]);
			k+=3;
		}	
	}
}

void BSP_LCD_ResetRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	BSP_LCD_ResetLine(x1,y1,x2,y1);
	BSP_LCD_ResetLine(x1,y1,x1,y2);
	BSP_LCD_ResetLine(x1,y2,x2,y2);
	BSP_LCD_ResetLine(x2,y1,x2,y2);
}

void BSP_LCD_ResetCircle(uint16_t x0,uint16_t y0,uint16_t r)
{
	int16_t a,b;
	int16_t di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
	while(a<=b)
	{
		BSP_LCD_ResetPiont(x0+a,y0-b);             //5
 		BSP_LCD_ResetPiont(x0+b,y0-a);             //0           
		BSP_LCD_ResetPiont(x0+b,y0+a);             //4               
		BSP_LCD_ResetPiont(x0+a,y0+b);             //6 
		BSP_LCD_ResetPiont(x0-a,y0+b);             //1       
 		BSP_LCD_ResetPiont(x0-b,y0+a);             
		BSP_LCD_ResetPiont(x0-a,y0-b);             //2             
  	BSP_LCD_ResetPiont(x0-b,y0-a);             //7     	         
		a++;
		//使用Bresenham算法画圆     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 						    
	}
}
#endif
