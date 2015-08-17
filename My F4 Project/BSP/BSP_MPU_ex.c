#include "BSP.h"
#include "arm_math.h"

#define invSqrtf( iSq ) (1.0f/sqrtf((float)iSq))
#define squa( Sq )      (((float)Sq)*((float)Sq))
#define toRad( Math_D )	((float)(Math_D)*0.0174532925f)
#define toDeg( Math_R )	((float)(Math_R)*57.2957795f)

#define _SampleRateFreg_500Hz_

#ifdef _SampleRateFreg_400Hz_
  #define SampleRateFreg  ((uint16_t)400)        // 400Hz
  #define SampleRate      ((float)0.0025f)   // 2.5ms
  #define SampleRateHelf  ((float)0.00125f)  // 1.25ms
#endif

#ifdef _SampleRateFreg_500Hz_
  #define SampleRateFreg  ((uint16_t)500)        // 500Hz
  #define SampleRate      ((float)0.002f)    // 2.0ms
  #define SampleRateHelf  ((float)0.001f)    // 1.0ms
#endif

Quaternion NumQ = {1, 0, 0, 0};
EulerAngle AngE = {0};

#define Kp 15.0f
#define Ki 0.020f//0.02f

extern SensorAcc Acc;
extern SensorGyr Gyr;
extern SensorMag Mag;

void Quaternion_RungeKutta( Quaternion *pNumQ, float GyrX, float GyrY, float GyrZ, float helfTimes )
{
  pNumQ->q0 = pNumQ->q0 + (-pNumQ->q1*GyrX - pNumQ->q2*GyrY - pNumQ->q3*GyrZ)*helfTimes;
  pNumQ->q1 = pNumQ->q1 + ( pNumQ->q0*GyrX - pNumQ->q3*GyrY + pNumQ->q2*GyrZ)*helfTimes;
  pNumQ->q2 = pNumQ->q2 + ( pNumQ->q3*GyrX + pNumQ->q0*GyrY - pNumQ->q1*GyrZ)*helfTimes;
  pNumQ->q3 = pNumQ->q3 + (-pNumQ->q2*GyrX + pNumQ->q1*GyrY + pNumQ->q0*GyrZ)*helfTimes;
}
void Quaternion_Normalize( Quaternion *pNumQ )
{
  float Normalize = 0.0f;

  Normalize = invSqrtf(squa(pNumQ->q0) + squa(pNumQ->q1) + squa(pNumQ->q2) + squa(pNumQ->q3));

  pNumQ->q0 = pNumQ->q0*Normalize;
  pNumQ->q1 = pNumQ->q1*Normalize;
  pNumQ->q2 = pNumQ->q2*Normalize;
  pNumQ->q3 = pNumQ->q3*Normalize;
}
void Quaternion_ToAngE( Quaternion *pNumQ, EulerAngle *pAngE )
{
  float NumQ_T11 = pNumQ->q0*pNumQ->q0 + pNumQ->q1*pNumQ->q1 - pNumQ->q2*pNumQ->q2 - pNumQ->q3*pNumQ->q3;
  float NumQ_T12 = 2.0f*(pNumQ->q0*pNumQ->q3 + pNumQ->q1*pNumQ->q2);
  float NumQ_T13 = 2.0f*(pNumQ->q1*pNumQ->q3 - pNumQ->q0*pNumQ->q2);
  float NumQ_T23 = 2.0f*(pNumQ->q0*pNumQ->q1 + pNumQ->q2*pNumQ->q3);
  float NumQ_T33 = pNumQ->q0*pNumQ->q0 - pNumQ->q1*pNumQ->q1 - pNumQ->q2*pNumQ->q2 + pNumQ->q3*pNumQ->q3;

  pAngE->Pitch = -asinf(NumQ_T13);
  pAngE->Roll  = atan2f(NumQ_T23, NumQ_T33);
  pAngE->Yaw   = atan2f(NumQ_T12, NumQ_T11);
}
void AHRS_Init( Quaternion *pNumQ, EulerAngle *pAngE )
{
  pNumQ->q0 = 1.0f;
  pNumQ->q1 = 0.0f;
  pNumQ->q2 = 0.0f;
  pNumQ->q3 = 0.0f;

  pAngE->Pitch = 0.0f;
  pAngE->Roll  = 0.0f;
  pAngE->Yaw   = 0.0f;
}
void AHRS_Update( void )
{
//  float tempX = 0, tempY = 0;
  float Normalize;
  float gx, gy, gz;
// float hx, hy, hz;
// float wx, wy, wz;
// float bx, bz;
  float ErrX, ErrY, ErrZ;
  float AccX, AccY, AccZ;
  float GyrX, GyrY, GyrZ;
// float MegX, MegY, MegZ;
  float /*Mq11, Mq12, */Mq13,/* Mq21, Mq22, */Mq23,/* Mq31, Mq32, */Mq33;

  static float AngZ_Temp = 0.0f;
  static float exInt = 0.0f, eyInt = 0.0f, ezInt = 0.0f;

//   Mq11 = NumQ.q0*NumQ.q0 + NumQ.q1*NumQ.q1 - NumQ.q2*NumQ.q2 - NumQ.q3*NumQ.q3;
//   Mq12 = 2.0f*(NumQ.q1*NumQ.q2 + NumQ.q0*NumQ.q3);
  Mq13 = 2.0f*(NumQ.q1*NumQ.q3 - NumQ.q0*NumQ.q2);
//   Mq21 = 2.0f*(NumQ.q1*NumQ.q2 - NumQ.q0*NumQ.q3);
//   Mq22 = NumQ.q0*NumQ.q0 - NumQ.q1*NumQ.q1 + NumQ.q2*NumQ.q2 - NumQ.q3*NumQ.q3;
  Mq23 = 2.0f*(NumQ.q0*NumQ.q1 + NumQ.q2*NumQ.q3);
//   Mq31 = 2.0f*(NumQ.q0*NumQ.q2 + NumQ.q1*NumQ.q3);
//   Mq32 = 2.0f*(NumQ.q2*NumQ.q3 - NumQ.q0*NumQ.q1);
  Mq33 = NumQ.q0*NumQ.q0 - NumQ.q1*NumQ.q1 - NumQ.q2*NumQ.q2 + NumQ.q3*NumQ.q3;

  Normalize = invSqrtf(squa(Acc.TrueX) + squa(Acc.TrueY) + squa(Acc.TrueZ));      
  AccX = Acc.TrueX*Normalize;
  AccY = Acc.TrueY*Normalize;
  AccZ = Acc.TrueZ*Normalize;

// Normalize = invSqrtf(squa(Meg.TrueX) + squa(Meg.TrueY) + squa(Meg.TrueZ));
// MegX = Meg.TrueX*Normalize;
// MegY = Meg.TrueY*Normalize;
// MegZ = Meg.TrueZ*Normalize;

  gx = Mq13;
  gy = Mq23;
  gz = Mq33;

// hx = MegX*Mq11 + MegY*Mq21 + MegZ*Mq31;
// hy = MegX*Mq12 + MegY*Mq22 + MegZ*Mq32;
// hz = MegX*Mq13 + MegY*Mq23 + MegZ*Mq33;

// bx = sqrtf(squa(hx) + squa(hy));
// bz = hz;

// wx = bx*Mq11 + bz*Mq13;
// wy = bx*Mq21 + bz*Mq23;
// wz = bx*Mq31 + bz*Mq33;

  ErrX = (AccY*gz - AccZ*gy)/* + (MegY*wz - MegZ*wy)*/;
  ErrY = (AccZ*gx - AccX*gz)/* + (MegZ*wx - MegX*wz)*/;
  ErrZ = (AccX*gy - AccY*gx)/* + (MegX*wy - MegY*wx)*/;

  exInt = exInt + ErrX*Ki;
  eyInt = eyInt + ErrY*Ki;
  ezInt = ezInt + ErrZ*Ki;

  GyrX = toRad(Gyr.TrueX);
  GyrY = toRad(Gyr.TrueY);
  GyrZ = toRad(Gyr.TrueZ);

  GyrX = GyrX + Kp*ErrX + exInt;
  GyrY = GyrY + Kp*ErrY + eyInt;
  GyrZ = GyrZ + Kp*ErrZ + ezInt;

  Quaternion_RungeKutta(&NumQ, GyrX, GyrY, GyrZ, SampleRateHelf);
  Quaternion_Normalize(&NumQ);
  Quaternion_ToAngE(&NumQ, &AngE);

//  tempX    = ( Mag.X*arm_cos_f32(Mag.EllipseSita)+Mag.Y*arm_sin_f32(Mag.EllipseSita))/Mag.EllipseB;
//  tempY    = (-Mag.X*arm_sin_f32(Mag.EllipseSita)+Mag.Y*arm_cos_f32(Mag.EllipseSita))/Mag.EllipseA;
//  AngE.Yaw = atan2f(tempX, tempY);

  AngE.Pitch = toDeg(AngE.Pitch);
  AngE.Roll  = toDeg(AngE.Roll);
  AngE.Yaw   = toDeg(AngE.Yaw)+180.0f;

  /* ???? Complementary Filter */
  #define CF_A 0.9f
  #define CF_B 0.1f
  AngZ_Temp = AngZ_Temp + GyrZ*SampleRate;
  AngZ_Temp = CF_A*AngZ_Temp + CF_B*AngE.Yaw;
  if(AngZ_Temp>360.0f)
    AngE.Yaw = AngZ_Temp - 360.0f;
  else if(AngZ_Temp<0.0f)
    AngE.Yaw = AngZ_Temp + 360.0f;
  else
    AngE.Yaw = AngZ_Temp;
}
