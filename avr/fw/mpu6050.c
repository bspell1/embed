//===========================================================================
// Module:  mpu6050.c
// Purpose: MPU-6050 6-axis motion processor API
//
// Copyright © 2013
// Brent M. Spell. All rights reserved.
//
// This library is free software; you can redistribute it and/or modify it 
// under the terms of the GNU Lesser General Public License as published 
// by the Free Software Foundation; either version 3 of the License, or 
// (at your option) any later version. This library is distributed in the 
// hope that it will be useful, but WITHOUT ANY WARRANTY; without even the 
// implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
// See the GNU Lesser General Public License for more details. You should 
// have received a copy of the GNU Lesser General Public License along with 
// this library; if not, write to 
//    Free Software Foundation, Inc. 
//    51 Franklin Street, Fifth Floor 
//    Boston, MA 02110-1301 USA
//===========================================================================
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
#include <math.h>
//-------------------[      Project Include Files      ]-------------------//
#include "mpu6050.h"
#include "i2cmast.h"
//-------------------[       Module Definitions        ]-------------------//
#if I2C_BUFFER_SIZE < 15
#  error The I2C buffer size is too small for MPU-6050.
#endif
//===========================================================================
// MPU6050 REGISTER ADDRESSES
//===========================================================================
#define REGISTER_CONFIG             0x1A
#define REGISTER_GYROCONFIG         0x1B
#define REGISTER_ACCELCONFIG        0x1C
#define REGISTER_ACCEL_X            0x3B
#define REGISTER_ACCEL_Y            0x3D
#define REGISTER_ACCEL_Z            0x3F
#define REGISTER_ACCEL_START        REGISTER_ACCEL_X
#define REGISTER_TEMP               0x41
#define REGISTER_GYRO_X             0x43
#define REGISTER_GYRO_Y             0x45
#define REGISTER_GYRO_Z             0x47
#define REGISTER_GYRO_START         REGISTER_GYRO_X
#define REGISTER_SENSOR_START       REGISTER_ACCEL_START
#define REGISTER_PWR_MGMT_1         0x6B
//===========================================================================
// SAMPLE SCALE FACTORS
//===========================================================================
#define GYRO_SENSOR_RANGE           32750.0f
#define ACCEL_SENSOR_RANGE          32767.0f
#define TEMP_SENSOR_RANGE           340.0f
#define TEMP_SENSOR_OFFSET          35.0f
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: DecodeI16 >-----------------------------------------
// Purpose:    reads a big endian 16-bit signed integer from a buffer
// Parameters: pbData - buffer to read
//             nIndex - index into the buffer, in 16-bit units
// Returns:    the converted value
//---------------------------------------------------------------------------
static I16 DecodeI16 (PBYTE pbData, UI8 nIndex)
{
   return ((I16)pbData[2 * nIndex] << 8) | pbData[2 * nIndex + 1];
}
//-----------< FUNCTION: BeginReadRegister >---------------------------------
// Purpose:    begins an asynchronous read of an MPU6050 register over I2C
// Parameters: nRegister - register address
//             cbData    - the number of bytes to read
// Returns:    none
//---------------------------------------------------------------------------
static VOID BeginReadRegister (UI8 nRegister, UI8 cbData)
{
   I2cBeginSendRecv(MPU6050_I2CADDR, &nRegister, 1, cbData, NULL);
}
//-----------< FUNCTION: EndReadRegister >-----------------------------------
// Purpose:    ends an asynchronous read of an MPU6050 register over I2C
// Parameters: pvData - return register data via here
//             cbData - the number of bytes to read
// Returns:    pvData
//---------------------------------------------------------------------------
static PVOID EndReadRegister (PVOID pvData, UI8 cbData)
{
   I2cEndSendRecv(pvData, cbData);
   return pvData;
}
//-----------< FUNCTION: ReadRegister >--------------------------------------
// Purpose:    reads an MPU6050 register over I2C
// Parameters: nRegister - register address
//             pvData    - return register data via here
//             cbData    - the number of bytes to read
// Returns:    pvData
//---------------------------------------------------------------------------
static PVOID ReadRegister (UI8 nRegister, PVOID pvData, UI8 cbData)
{
   BeginReadRegister(nRegister, cbData);
   EndReadRegister(pvData, cbData);
   return pvData;
}
//-----------< FUNCTION: WriteRegister >-------------------------------------
// Purpose:    writes an MPU6050 register over I2C
// Parameters: nRegister - register address
//             pvData    - register data to write
//             cbData    - the number of bytes to write
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister (UI8 nRegister, PVOID pvData, UI8 cbData)
{
   UI8  cbSend = cbData + 1;
   BYTE pbSend[cbSend];
   pbSend[0] = nRegister;
   memcpy(pbSend + 1, pvData, cbData);
   I2cSend(MPU6050_I2CADDR, pbSend, cbSend);
}
//-----------< FUNCTION: ReadRegister8 >-------------------------------------
// Purpose:    reads an MPU6050 8-bit register over I2C
// Parameters: nRegister - register address
// Returns:    the current register value
//---------------------------------------------------------------------------
static UI8 ReadRegister8 (UI8 nRegister)
{
   UI8 nValue = 0;
   ReadRegister(nRegister, &nValue, 1);
   return nValue;
}
//-----------< FUNCTION: WriteRegister8 >------------------------------------
// Purpose:    writes an MPU6050 8-bit register over I2C
// Parameters: nRegister - register address
//             nValue    - register value to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister8 (UI8 nRegister, UI8 nValue)
{
   WriteRegister(nRegister, &nValue, 1);
}
//-----------< FUNCTION: ReadRegisterI16 >-----------------------------------
// Purpose:    reads an MPU6050 signed 16-bit big endian register over I2C
// Parameters: nRegister - register address
// Returns:    the current register value
//---------------------------------------------------------------------------
static I16 ReadRegisterI16 (UI8 nRegister)
{
   BYTE pbBuffer[2];
   ReadRegister(nRegister, pbBuffer, 2);
   return DecodeI16(pbBuffer, 0);
}
//-----------< FUNCTION: Mpu6050Init >---------------------------------------
// Purpose:    MPU6050 interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050Init ()
{
   // reset the module
   Mpu6050Reset();
}
//-----------< FUNCTION: Mpu6050GetFrameSync >-------------------------------
// Purpose:    reads the frame sync configuration register
// Parameters: none
// Returns:    the configured MPU6050_FRAMESYNC_* value
//---------------------------------------------------------------------------
UI8 Mpu6050GetFrameSync ()
{
   return (ReadRegister8(REGISTER_CONFIG) >> 3) & 0x7;
}
//-----------< FUNCTION: Mpu6050SetFrameSync >-------------------------------
// Purpose:    writes the frame sync configuration register
// Parameters: nFrameSync - the MPU6050_FRAMESYNC_* value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetFrameSync (UI8 nFrameSync)
{
   WriteRegister8(
      REGISTER_CONFIG,
      (ReadRegister8(REGISTER_CONFIG) & ~(0x7 << 3)) | ((nFrameSync & 0x7) << 3)
   );
}
//-----------< FUNCTION: Mpu6050GetLowPassFilter >---------------------------
// Purpose:    reads the low-pass filter configuration register
// Parameters: none
// Returns:    the configured MPU6050_DLPF_* value
//---------------------------------------------------------------------------
UI8 Mpu6050GetLowPassFilter ()
{
   return ReadRegister8(REGISTER_CONFIG) & 0x7;
}
//-----------< FUNCTION: Mpu6050SetLowPassFilter >---------------------------
// Purpose:    writes the low-pass filter configuration register
// Parameters: nFilter - the MPU6050_DLPF_* value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetLowPassFilter (UI8 nFilter)
{
   WriteRegister8(
      REGISTER_CONFIG,
      (ReadRegister8(REGISTER_CONFIG) & ~0x7) | (nFilter & 7)
   );
}
//-----------< FUNCTION: Mpu6050GetGyroSelfTest >----------------------------
// Purpose:    reads the gyroscope self-test flag register
// Parameters: nAxis - the axis to read (MPU6050_AXIS_*)
// Returns:    the gyro self-test flag for the specified axis
//---------------------------------------------------------------------------
BOOL Mpu6050GetGyroSelfTest (UI8 nAxis)
{
   return BitTest(ReadRegister8(REGISTER_GYROCONFIG), 7 - nAxis);
}
//-----------< FUNCTION: Mpu6050SetGyroSelfTest >----------------------------
// Purpose:    writes the gyroscope self-test flag register
// Parameters: nAxis - the axis to write (MPU6050_AXIS_*)
//             fTest - the self-test flag to write
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetGyroSelfTest (UI8 nAxis, BOOL fTest)
{
   WriteRegister8(
      REGISTER_GYROCONFIG,
      BitSet(ReadRegister8(REGISTER_GYROCONFIG), 7 - nAxis, fTest)
   );
}
//-----------< FUNCTION: Mpu6050GetGyroScale >-------------------------------
// Purpose:    reads the gyroscope range/scale register
// Parameters: none
// Returns:    the gyro scale configuration (MPU6050_GYRO_SCALE_*)
//---------------------------------------------------------------------------
UI8 Mpu6050GetGyroScale ()
{
   return (ReadRegister8(REGISTER_GYROCONFIG) >> 3) & 0x3;
}
//-----------< FUNCTION: Mpu6050SetGyroScale >-------------------------------
// Purpose:    writes the gyroscope range/scale register
// Parameters: nScale - the scale value to write (MPU6050_GYRO_SCALE_*)
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetGyroScale (UI8 nScale)
{
   WriteRegister8(
      REGISTER_GYROCONFIG,
      (ReadRegister8(REGISTER_GYROCONFIG) & ~(0x3 << 3)) | ((nScale & 0x3) << 3)
   );
}
//-----------< FUNCTION: Mpu6050GetAccelSelfTest >---------------------------
// Purpose:    reads the accelerometer self-test flag register
// Parameters: nAxis - the axis to read (MPU6050_AXIS_*)
// Returns:    the accel self-test flag for the specified axis
//---------------------------------------------------------------------------
BOOL Mpu6050GetAccelSelfTest (UI8 nAxis)
{
   return BitTest(ReadRegister8(REGISTER_ACCELCONFIG), 7 - nAxis);
}
//-----------< FUNCTION: Mpu6050SetAccelSelfTest >---------------------------
// Purpose:    writes the accelerometer self-test flag register
// Parameters: nAxis - the axis to write (MPU6050_AXIS_*)
//             fTest - the self-test flag to write
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetAccelSelfTest (UI8 nAxis, BOOL fTest)
{
   WriteRegister8(
      REGISTER_ACCELCONFIG,
      BitSet(ReadRegister8(REGISTER_ACCELCONFIG), 7 - nAxis, fTest)
   );
}
//-----------< FUNCTION: Mpu6050GetAccelScale >------------------------------
// Purpose:    reads the accelerometer range/scale register
// Parameters: none
// Returns:    the accel scale configuration (MPU6050_ACCEL_SCALE_*)
//---------------------------------------------------------------------------
UI8 Mpu6050GetAccelScale ()
{
   return (ReadRegister8(REGISTER_ACCELCONFIG) >> 3) & 0x3;
}
//-----------< FUNCTION: Mpu6050SetAccelScale >------------------------------
// Purpose:    writes the accelerometer range/scale register
// Parameters: nScale - the scale value to write (MPU6050_ACCEL_SCALE_*)
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetAccelScale (UI8 nScale)
{
   WriteRegister8(
      REGISTER_ACCELCONFIG,
      (ReadRegister8(REGISTER_ACCELCONFIG) & ~(0x3 << 3)) | ((nScale & 0x3) << 3)
   );
}
//-----------< FUNCTION: Mpu6050ReadAccelAxis >------------------------------
// Purpose:    reads an accelerometer sensor
// Parameters: nAxis - the axis to read (MPU6050_AXIS_*)
// Returns:    the requested accelerometer measurement, in the range [-1,1]
//---------------------------------------------------------------------------
F32 Mpu6050ReadAccelAxis (UI8 nAxis)
{
   return (F32)ReadRegisterI16(REGISTER_ACCEL_START + nAxis * 2) / ACCEL_SENSOR_RANGE;
}
//-----------< FUNCTION: Mpu6050ReadAccel >----------------------------------
// Purpose:    reads the three accelerometer sensors
// Parameters: pAccel - return the result via here
//                      a vector containing the 3 accelerometer sensors, 
//                      each in the range [-1,1]
// Returns:    pAccel
//---------------------------------------------------------------------------
MPU6050_VECTOR* Mpu6050ReadAccel (MPU6050_VECTOR* pAccel)
{
   BYTE pbBuffer[6];
   ReadRegister(REGISTER_ACCEL_START, pbBuffer, 6);
   for (UI8 i = 0; i < 3; i++)
      pAccel->v[i] = (F32)DecodeI16(pbBuffer, i) / ACCEL_SENSOR_RANGE;
   return pAccel;
}
//-----------< FUNCTION: Mpu6050ReadTempCelsius >----------------------------
// Purpose:    reads the temperature sensor
// Parameters: none
// Returns:    the current temperature reading, in degrees Celsius
//---------------------------------------------------------------------------
F32 Mpu6050ReadTempCelsius ()
{
   return (F32)ReadRegisterI16(REGISTER_TEMP) / TEMP_SENSOR_RANGE + TEMP_SENSOR_OFFSET;
}
//-----------< FUNCTION: Mpu6050ReadTempFahrenheit >-------------------------
// Purpose:    reads the temperature sensor
// Parameters: none
// Returns:    the current temperature reading, in degrees Fahrenheit
//---------------------------------------------------------------------------
F32 Mpu6050ReadTempFahrenheit ()
{
   return Mpu6050ReadTempCelsius() * 9.0f / 5.0f + 32.0f;
}
//-----------< FUNCTION: Mpu6050ReadGyroAxis >-------------------------------
// Purpose:    reads a gyroscope sensor
// Parameters: nAxis - the axis to read (MPU6050_AXIS_*)
// Returns:    the requested gyroscope measurement, in the range [-1,1]
//---------------------------------------------------------------------------
F32 Mpu6050ReadGyroAxis (UI8 nAxis)
{
   return (F32)ReadRegisterI16(REGISTER_GYRO_START + nAxis * 2) / GYRO_SENSOR_RANGE;
}
//-----------< FUNCTION: Mpu6050ReadGyro >-----------------------------------
// Purpose:    reads the three gyroscope sensors
// Parameters: pGyro - return the result via here
//                     a vector containing the 3 gyroscope sensors, 
//                     each in the range [-1,1]
// Returns:    pGyro
//---------------------------------------------------------------------------
MPU6050_VECTOR* Mpu6050ReadGyro (MPU6050_VECTOR* pGyro)
{
   BYTE pbBuffer[6];
   ReadRegister(REGISTER_GYRO_START, pbBuffer, 6);
   for (UI8 i = 0; i < 3; i++)
      pGyro->v[i] = (F32)DecodeI16(pbBuffer, i) / GYRO_SENSOR_RANGE;
   return pGyro;
}
//-----------< FUNCTION: Mpu6050BeginReadSensors >---------------------------
// Purpose:    begins an asynchronous read of all sensors from the 
//             MPU-6050 in one I2C transaction
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050BeginReadSensors ()
{
   BeginReadRegister(REGISTER_SENSOR_START, 14);
}
//-----------< FUNCTION: Mpu6050EndReadSensors >-----------------------------
// Purpose:    completes an asynchronous read of all sensors from the 
//             MPU-6050 in one I2C transaction
// Parameters: pSensors - return the result via here
//                        a structure containing the sensor readings
// Returns:    pSensors
//---------------------------------------------------------------------------
MPU6050_SENSORS* Mpu6050EndReadSensors (MPU6050_SENSORS* pSensors)
{
   // complete the async read
   BYTE pbBuffer[14];
   EndReadRegister(pbBuffer, sizeof(pbBuffer));
   // decode the sensor readings from the buffer
   for (UI8 i = 0; i < 3; i++)
      pSensors->Accel.v[i] = (F32)DecodeI16(pbBuffer, i) / ACCEL_SENSOR_RANGE;
   pSensors->Temp = (F32)DecodeI16(pbBuffer, 3) / TEMP_SENSOR_RANGE + TEMP_SENSOR_OFFSET;
   for (UI8 i = 0; i < 3; i++)
      pSensors->Gyro.v[i] = (F32)DecodeI16(pbBuffer, i + 4) / GYRO_SENSOR_RANGE;
   return pSensors;
}
//-----------< FUNCTION: Mpu6050Reset >--------------------------------------
// Purpose:    resets the configuration of the MPU-6050 to power-on state
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050Reset ()
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1, 
      BitSetHi(ReadRegister8(REGISTER_PWR_MGMT_1), 7)
   );
   // the module requires a delay upon reset, say 5ms
   _delay_ms(5);
}
//-----------< FUNCTION: Mpu6050IsAsleep >-----------------------------------
// Purpose:    determines whether the motion processor is asleep
// Parameters: none
// Returns:    true if the MPU is asleep
//             false otherwise
//---------------------------------------------------------------------------
BOOL Mpu6050IsAsleep ()
{
   return BitTest(ReadRegister8(REGISTER_PWR_MGMT_1), 6);
}
//-----------< FUNCTION: Mpu6050Sleep >--------------------------------------
// Purpose:    puts the MPU to sleep
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050Sleep ()
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1,
      BitSetHi(ReadRegister8(REGISTER_PWR_MGMT_1), 6)
   );
}
//-----------< FUNCTION: Mpu6050Wake >---------------------------------------
// Purpose:    wakes the MPU from the sleeping state
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050Wake ()
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1,
      BitSetLo(ReadRegister8(REGISTER_PWR_MGMT_1), 6)
   );
   // the module requires a delay upon waking, say 5ms
   _delay_ms(5);
}
//-----------< FUNCTION: Mpu6050IsCycling >----------------------------------
// Purpose:    determines whether the MPU is in low-power 
//             accelerometer-only power cycling mode
// Parameters: none
// Returns:    true if the MPU is in cycling mode
//             false otherwise
//---------------------------------------------------------------------------
BOOL Mpu6050IsCycling ()
{
   return BitTest(ReadRegister8(REGISTER_PWR_MGMT_1), 5);
}
//-----------< FUNCTION: Mpu6050SetCycling >---------------------------------
// Purpose:    puts the MPU into low-power cycling mode, or
//             restores it to normal operation
// Parameters: bCycling - true to put the MPU into cycling mode
//                        false to take the MPU out of cycling mode
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetCycling (BOOL bCycling)
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1,
      BitSet(ReadRegister8(REGISTER_PWR_MGMT_1), 5, bCycling)
   );
}
//-----------< FUNCTION: Mpu6050IsTempDisabled >-----------------------------
// Purpose:    determines whether the temperature sensor is disabled
// Parameters: none
// Returns:    true if the temperature sensor is disabled
//             false otherwise
//---------------------------------------------------------------------------
BOOL Mpu6050IsTempDisabled ()
{
   return BitTest(ReadRegister8(REGISTER_PWR_MGMT_1), 3);
}
//-----------< FUNCTION: Mpu6050SetTempDisabled >----------------------------
// Purpose:    disables/enables the temperature sensor
// Parameters: fDisabled - true to disable the sensor
//                         false to enable the sensor
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetTempDisabled (BOOL fDisabled)
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1,
      BitSet(ReadRegister8(REGISTER_PWR_MGMT_1), 3, fDisabled)
   );
}
//-----------< FUNCTION: Mpu6050GetClockSource >-----------------------------
// Purpose:    reads the MPU clock source register
// Parameters: none
// Returns:    the current clock source (MPU6050_CLOCK_*)
//---------------------------------------------------------------------------
UI8 Mpu6050GetClockSource ()
{
   return ReadRegister8(REGISTER_PWR_MGMT_1) & 0x7;
}
//-----------< FUNCTION: Mpu6050SetClockSource >-----------------------------
// Purpose:    writes the MPU clock source register
// Parameters: nSource - the clock source to assign (MPU6050_CLOCK_*)
// Returns:    none
//---------------------------------------------------------------------------
VOID Mpu6050SetClockSource (UI8 nSource)
{
   WriteRegister8(
      REGISTER_PWR_MGMT_1,
      (ReadRegister8(REGISTER_PWR_MGMT_1) & ~0x7) | (nSource & 0x7)
   );
}
