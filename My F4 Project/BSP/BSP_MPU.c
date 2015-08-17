#include "BSP.h"

//static void BSP_MPU_ReadReg(uint8_t Addr,uint8_t * ReadData);
//static void BSP_MPU_WriteReg(uint8_t Addr,uint8_t WriteData);

SensorAcc Acc = {0};
SensorGyr Gyr = {0};
SensorMag Mag = {0};

static const uint8_t MPU_Init[][2] = 
{
//	/* MPU6500 Step */
//	{0x80, MPU6500_PWR_MGMT_1},     // Reset Device
//	{0x01, MPU6500_PWR_MGMT_1},     // Clock Source
//	{0x00, MPU6500_PWR_MGMT_2},     // Enable Acc & Gyro
//	{0x07, MPU6500_CONFIG},         // 
//	{0x18, MPU6500_GYRO_CONFIG},    // +-2000dps
//	{0x08, MPU6500_ACCEL_CONFIG},   // +-4G
//	{0x00, MPU6500_ACCEL_CONFIG_2}, // Set Acc Data Rates
//	{0x30, MPU6500_INT_PIN_CFG},    // 
//	
//	{0x0d, MPU6500_I2C_MST_CTRL},   // I2C Speed 400 kHz
//	{0x20, MPU6500_USER_CTRL},      // Enable AUX

//	/* AK8963 Step */
//	{0x8C, MPU6500_I2C_SLV0_ADDR},  // AK8963_I2C_ADDR ( 7'b000_1100 )
//	{0x00, MPU6500_I2C_SLV0_REG},   // AK8963_WIA ( 0x00 )
//	{0x81, MPU6500_I2C_SLV0_CTRL},  // Enable
//	//{0x01, MPU6500_I2C_MST_DELAY_CTRL}
	
		/* MPU6500 Step */
	{0x80, MPU6500_PWR_MGMT_1},     // Reset Device
	{0x01, MPU6500_PWR_MGMT_1},     // Clock Source
	{0x00, MPU6500_PWR_MGMT_2},     // Enable Acc & Gyro
	{0x07, MPU6500_CONFIG},         // 
	{0x18, MPU6500_GYRO_CONFIG},    // +-2000dps
	{0x08, MPU6500_ACCEL_CONFIG},   // +-4G
	{0x00, MPU6500_ACCEL_CONFIG_2}, // Set Acc Data Rates
	{0x30, MPU6500_INT_PIN_CFG},    // 
	
	{0x40, MPU6500_I2C_MST_CTRL},   // I2C Speed 400 kHz
	{0x20, MPU6500_USER_CTRL},      // Enable AUX

	/* AK8963 Step */
	{0x0C, MPU6500_I2C_SLV0_ADDR},  // AK8963_I2C_ADDR ( 7'b000_1100 )
	{0x0B, MPU6500_I2C_SLV0_REG},   // 
	{0x01, MPU6500_I2C_SLV0_DO},	
	{0x81, MPU6500_I2C_SLV0_CTRL},  // Enable
	//{0x01, MPU6500_I2C_MST_DELAY_CTRL}
	{0x0A, MPU6500_I2C_SLV0_REG},
	{0x12, MPU6500_I2C_SLV0_DO},
	{0x81, MPU6500_I2C_SLV0_CTRL},
};

void MPU9250_ReadReg(uint8_t Addr,uint8_t * ReadData)
{
	uint8_t txData[1];
	
	txData[0] = Addr|0x80;
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);		//MPU9250 Chip Select
	HAL_SPI_Transmit(&hspi1,txData,1,1);
	HAL_SPI_Receive(&hspi1,ReadData,1,1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}

void MPU9250_WriteReg(uint8_t Addr,uint8_t WriteData)
{
	uint8_t txData[2];
	
	txData[0] = Addr;
	txData[1] = WriteData;
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);		//MPU9250 Chip Select
	HAL_SPI_Transmit(&hspi1,txData,2,1);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}

void MPU9250_Read(uint8_t * ReadBuf)
{
	uint8_t txData[1];
	
	txData[0] = MPU6500_ACCEL_XOUT_H|0x80;
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);		//MPU9250 Chip Select
	HAL_SPI_Transmit(&hspi1,txData,1,1);
	HAL_SPI_Receive(&hspi1,ReadBuf,14,1);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}
void AK8963_Read(uint8_t * ReadBuf)
{
	uint8_t txData[1];
	
	//txData[0] = MPU6500_EXT_SENS_DATA_00|0x80;
	txData[0] = MPU6500_ACCEL_XOUT_H|0x80;
	
	MPU9250_WriteReg(MPU6500_I2C_SLV0_ADDR,0x8c);
	HAL_Delay(1);
	MPU9250_WriteReg(MPU6500_I2C_SLV0_REG,AK8963_HXL);
	HAL_Delay(1);
	MPU9250_WriteReg(MPU6500_I2C_SLV0_CTRL,0x87);
	HAL_Delay(1);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET);		//MPU9250 Chip Select
	
	HAL_SPI_Transmit(&hspi1,txData,1,1);
	HAL_SPI_Receive(&hspi1,ReadBuf,22,1);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);
}
void MPU_MPU9250_Init(void)
{
	uint16_t size = sizeof(MPU_Init)/sizeof(MPU_Init[0][0])/2;
	uint16_t i;

//	MPU9250_WriteReg(MPU6500_PWR_MGMT_1,0x80);	//Reset Device

//	HAL_Delay(10);

	for(i=0;i<size;i++)
	{
		MPU9250_WriteReg(MPU_Init[i][1],MPU_Init[i][0]);
		HAL_Delay(1);
	}
}	
uint8_t MPU_MPU9250_Check( void )
{
  uint8_t DeviceID = 0x00;
//MPU_MPU9250_Init();
  /* MPU6500 */
  DeviceID = 0x00;
  MPU9250_ReadReg(MPU6500_WHO_AM_I, &DeviceID);
  if(DeviceID != MPU6500_Device_ID)
    return ERROR;

  /* AK8975 */
  DeviceID = 0x00;
  MPU9250_WriteReg(MPU6500_I2C_SLV0_ADDR, 0x8c);          // Set AK8963_I2C_ADDR = 7'b000_1100
	HAL_Delay(1);
  MPU9250_WriteReg(MPU6500_I2C_SLV0_REG, AK8963_WIA);     // Set Write Reg
  HAL_Delay(1);
	MPU9250_WriteReg(MPU6500_I2C_SLV0_CTRL, 0x81);          // Start Read
  HAL_Delay(1);
  MPU9250_ReadReg(MPU6500_EXT_SENS_DATA_00, &DeviceID);   // Read Data
	
  if(DeviceID != AK8963_Device_ID)
    return ERROR;

  return SUCCESS;
}
void MPU_MS5611_Init(void)
{
	
}

void MPU_SPI_Init(void)
{
	hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  HAL_SPI_Init(&hspi1);
}

void MPU_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	
	__GPIOE_CLK_ENABLE();
	
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;

	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET);			//MPU9250	CS
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET);			//MS5611	CS
	//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_RESET);		//ADC			CS
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
}
/*-------------------------------------------------------
Copyright : yitiandelan.
---------------------------------------------------------*/
void BSP_MPU_Init(void)
{
	MPU_GPIO_Init();
	
	
	__GPIOB_CLK_ENABLE();
	MPU_SPI_Init();
	__HAL_SPI_ENABLE(&hspi1);
	
	MPU_MPU9250_Init();
	while(MPU_MPU9250_Check()!=SUCCESS);
	MPU_MS5611_Init();
}
uint8_t ReadBuf[24];
void BSP_MPU_Read(void)
{
	
	
	//MPU9250_Read(ReadBuf);
	AK8963_Read(ReadBuf);
	
	Acc.X  = (ReadBuf[0] << 8) | ReadBuf[1];		// Acc.X
	Acc.Y  = (ReadBuf[2] << 8) | ReadBuf[3];		// Acc.Y
	Acc.Z  = (ReadBuf[4] << 8) | ReadBuf[5];		// Acc.Z
	Gyr.X  = (ReadBuf[8] << 8) | ReadBuf[9];		// Gyr.X
	Gyr.Y  = (ReadBuf[10] << 8) | ReadBuf[11];	// Gyr.Y
	Gyr.Z  = (ReadBuf[12] << 8) | ReadBuf[13];	// Gyr.Z
	
	Mag.X  = (ReadBuf[15] << 8) | ReadBuf[14];	// Mag.X
  Mag.Y  = (ReadBuf[17] << 8) | ReadBuf[16];	// Mag.Y
  Mag.Z  = (ReadBuf[19] << 8) | ReadBuf[18];	// Mag.Z
	
	Acc.TrueX  = Acc.X*MPU9250A_4g;       // g/LSB
  Acc.TrueY  = Acc.Y*MPU9250A_4g;       // g/LSB
  Acc.TrueZ  = Acc.Z*MPU9250A_4g;       // g/LSB
  Gyr.TrueX  = Gyr.X*MPU9250G_2000dps;  // dps/LSB
  Gyr.TrueY  = Gyr.Y*MPU9250G_2000dps;  // dps/LSB
  Gyr.TrueZ  = Gyr.Z*MPU9250G_2000dps;  // dps/LSB

  Mag.TrueX  = Mag.X*MPU9250M_4800uT;   // uT/LSB
  Mag.TrueY  = Mag.Y*MPU9250M_4800uT;   // uT/LSB
  Mag.TrueZ  = Mag.Z*MPU9250M_4800uT;   // uT/LSB
}
