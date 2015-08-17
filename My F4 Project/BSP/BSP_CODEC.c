#include "BSP.h"
#include "math.h"

#define I2C_ADDRESS		(0x18<<1)

I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;
DMA_HandleTypeDef hdma_i2s3_ext_rx;

void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data);
uint8_t Single_ReadI2C(uint8_t REG_Address);

int16_t I2S_ADCBuf[1000];
int16_t I2S_DACBuf[1000];


static const uint8_t CodecInit[][2] = 
{
	/* DAC Step */
	{0x00,0x00},	//Select Page 0
	{0x0b,0x81},	//NDAC = 1,Power up
	{0x0c,0x82},	//MDAC = 2,Power up
	{0x0d,0x00},	//DOSR = 128
	{0x0e,0x80},	//DOSR LSB
	//{0x3c,0x08},	//PRB_P8
	
	/* ADC Step */
	{0x12,0x81},	//NADC = 1,Power up
	{0x13,0x82},	//MADC = 2,Power up
	{0x14,0x80},	//AOSR = 128
	{0x3d,0x01},	//PRB_R1
	
	/* Audio Interface */
	{0x1b,0x00},	//16bit,PTM_P4
	
	/* Power Step */
	{0x00,0x01},	//Select Page 1
	{0x01,0x08},	//Disabled weak connection of AVDD with DVDD
	{0x02,0x01},	//Analog Block Power up,AVDD LDO Power up
	//{0x0a,0x3b},	//Input Vcom = 0.9v,Output Vcom = 1.65v
	{0x0a,0x03},
	
	{0x03,0x00},	//DAC PTM mode		to PTM_P3/4
	{0x04,0x00},
	{0x3d,0x00},	//ADC PTM mode		to PTM_R4
	
//	{0x7b,0x01},	//REF settime			to 40ms
//	{0x14,0x25},	//HP settime			to	
	
	/* Input Step */
	{0x34,0x20},	//Route IN2L to LEFT_P with 20k
	{0x36,0x20},	//Route IN2R to LEFT_M with 20k
	{0x37,0x80},	//Route IN1R to RIGHT_P with 20k
	{0x39,0x20},	//Route IN1L to RIGHT_M with 20k
	
	{0x3b,0x00},	//Left  MicPGA not mute,gain to 0dB
	{0x3c,0x00},	//Right MicPGA not mute,gain to 0dB
	
	/* Output Step */
	{0x0c,0x08},	//Route Left  DAC to HPL
	{0x0d,0x08},	//Route Right DAC to HPR
	{0x0e,0x08},	//Route Left  DAC to LOL
	{0x0f,0x08},	//Route Right DAC to LOR
	
	{0x10,0x00},	//HPL gain	to 0dB
	{0x11,0x00},	//HPR gain	to 0dB
	{0x12,0x08},	//LOL gain	to 0dB
	{0x13,0x08},	//LOR gain	to 0dB
	
	{0x16,0x75},
	{0x17,0x75},
	
	{0x09,0x3c},	//LOL,LOR,HPL,HPR,Power up
	
	/* Initial ok */
	{0x00,0x00},	//Select Page 0
	{0x3f,0xd6},	//L&R DAC Power up
	{0x40,0x00},	//L&R DAC not mute
	{0x51,0xc0},	//L&R ADC Power up
	{0x52,0x00},	//L&R ADC not mute
};

//void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef* hi2s)
//{
//	//CDC_Transmit_FS((uint8_t *)&I2S_ADCBuf+1000,1000);
//	USBD_CDC_SetTxBuffer(&hUsbDeviceFS,(uint8_t *)&I2S_ADCBuf+1000,1000);
//	VCPSentFlag = 1;
//}

//void HAL_I2S_RxHalfCpltCallback(I2S_HandleTypeDef* hi2s)
//{
//	//CDC_Transmit_FS((uint8_t *)&I2S_ADCBuf,1000);
//	USBD_CDC_SetTxBuffer(&hUsbDeviceFS,(uint8_t *)&I2S_ADCBuf,1000);
//	VCPSentFlag = 1;
//}

void BSP_CODEC_Init(void)
{
	uint16_t size = sizeof(CodecInit)/sizeof(CodecInit[0][0])/2;
	uint16_t i;
	
	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();
	
	{
		GPIO_InitTypeDef GPIO_InitStruct;
		
		__GPIOD_CLK_ENABLE();
		
		GPIO_InitStruct.Pin = GPIO_PIN_2;
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
		HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
	}
	{
		__DMA1_CLK_ENABLE();

		HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
		HAL_NVIC_SetPriority(DMA1_Stream7_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream7_IRQn);
	}
	{
		hi2s3.Instance = SPI3;
		hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
		hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
		hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
		hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
		hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_96K;
		hi2s3.Init.CPOL = I2S_CPOL_LOW;
		hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
		hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_ENABLE;
		HAL_I2S_Init(&hi2s3);
	}
	{
		hi2c1.Instance = I2C1;
		hi2c1.Init.ClockSpeed = 100000;
		hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
		hi2c1.Init.OwnAddress1 = 0;
		hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLED;
		hi2c1.Init.OwnAddress2 = 0;
		hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
		hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
		HAL_I2C_Init(&hi2c1);
	}
	
	/* TLV320AIC3204 reset */
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	
	HAL_Delay(2);
	
	for(i=0;i<size;i++)
			Single_WriteI2C(CodecInit[i][0],CodecInit[i][1]);
}

void BSP_CODEC_Start(void)
{
	int16_t i;
	for(i=0;i<500;i++)
	{
		//I2S_DACBuf[2*i] = 0;
		I2S_DACBuf[2*i+1] = 30000*sin(2*3.14159265359*i*5.0/500.0);
		I2S_DACBuf[2*i] = 0;
	}
	
	HAL_I2SEx_TransmitReceive_DMA(&hi2s3,(uint16_t *)&I2S_DACBuf,(uint16_t *)&I2S_ADCBuf,1000);
}

void Single_WriteI2C(uint8_t REG_Address,uint8_t REG_data)
{
    uint8_t txData[2] = {REG_Address,REG_data};
    while(HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDRESS,txData,2,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
}
uint8_t Single_ReadI2C(uint8_t REG_Address)
{
    uint8_t REG_data;
    while(HAL_I2C_Master_Transmit(&hi2c1,I2C_ADDRESS,&REG_Address,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
    
    if(HAL_I2C_Master_Receive(&hi2c1,I2C_ADDRESS+1,&REG_data,1,100) != HAL_OK)
    {
        if(HAL_I2C_GetError(&hi2c1) != HAL_I2C_ERROR_AF)
        {}
    }
    return REG_data;
}
