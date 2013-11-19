//===========================================================================
// Module:  quadmpu.c
// Purpose: quadcopter motion processor and sensor filter
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
//-------------------[      Project Include Files      ]-------------------//
#include "quadmpu.h"
#include "mpu6050.h"
//-------------------[       Module Definitions        ]-------------------//
// complementary filter biases
#define COMPFILTER_GYROBIAS   (0.95f)
#define COMPFILTER_ACCELBIAS  (1.0f - COMPFILTER_GYROBIAS)
// MPU-6050 reading scales
#define ACCEL_SCALE           (2.0f * M_PI_2)            // [-2g,2g] => radians
#define GYRO_SCALE            (250.0f / 180.0f * M_PI)   // [-250deg/sec,250deg/sec] => rad/sec
//-------------------[        Module Variables         ]-------------------//
// complementary filter state
static F32 g_nFilterX = 0.0f;
static F32 g_nFilterY = 0.0f;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: ComplementaryFilter >-------------------------------
// Purpose:    accelerometer/gyroscope complementary digital filter
//             smooths an accelerometer-based tilt angle by integrating
//             a gyroscope reading over time
// Reference:  http://web.mit.edu/scolton/www/filter.pdf
// Parameters: nOldAngle   - previous filter value (radians)
//             nAccelAngle - accelerometer-based tilt angle (radians)
//             nGyroRate   - gyroscope-based angular velocity (rad/sec)
//             nDt         - seconds elapsed since the previous reading
// Returns:    the filtered tilt angle measurement
//---------------------------------------------------------------------------
static F32 ComplementaryFilter (
   F32 nOldAngle, 
   F32 nAccelAngle, 
   F32 nGyroRate, 
   F32 nDt)
{
   // calculate the filter
   // . integrate the gyroscope angular velocity reading over time
   //   and add it to the previous filtered tilt angle
   // . combine this reading with the current accelerometer-based tilt
   //   angle using a weighted average that biases the gyroscope-based
   //   calculation over the noisy accelerometer calculation
   // . clamp the result to [-PI/2,PI/2]
   F32 nGyroAngle = nOldAngle + nGyroRate * nDt;
   F32 nNewAngle  = COMPFILTER_GYROBIAS * nGyroAngle + 
                    COMPFILTER_ACCELBIAS * nAccelAngle;
   return Min(Max(nNewAngle, -M_PI_2), M_PI_2);
}
//-----------< FUNCTION: QuadMpuInit >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadMpuInit (PQUADMPU_CONFIG pConfig)
{
   IgnoreParam(pConfig);
   Mpu6050Init();
   Mpu6050Wake();
   Mpu6050DisableTemp();
   Mpu6050SetClockSource(MPU6050_CLOCK_PLLGYROX);
}
//-----------< FUNCTION: QuadMpuBeginRead >----------------------------------
// Purpose:    begins an asynchronous read of the MPU sensors
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadMpuBeginRead ()
{
   Mpu6050BeginReadSensors();
}
//-----------< FUNCTION: QuadMpuEndRead >------------------------------------
// Purpose:    completes an asynchronous read of the MPU sensors
//             and filters the results
// Parameters: pSensor - return the sensor readings via here
// Returns:    pSensor
//---------------------------------------------------------------------------
QUADMPU_SENSOR* QuadMpuEndRead (PQUADMPU_SENSOR pSensor)
{
   // complete the async sensor read
   MPU6050_SENSORS mpu;
   Mpu6050EndReadSensors(&mpu);
   // scale the sensor readings
   // . accelerometer default raw range is 0g-2g, convert to radians
   // . gyroscope default raw range is 250 deg/sec, convert to radians/sec
   // . convert the sinusoidal accelerometer readings to an angle
   //   using the small-angle approximation: a ~= sin(a)
   //   this should be acceptable, since large angles
   //   cause problems with atan2-based methods anyway, 
   //   and they also cause quadcopters to crash
   F32 nAngleX = mpu.Accel.x * ACCEL_SCALE;
   F32 nAngleY = mpu.Accel.y * ACCEL_SCALE;
   F32 nRateX  = mpu.Gyro.x * GYRO_SCALE;
   F32 nRateY  = mpu.Gyro.y * GYRO_SCALE;
   // filter the angle readings using the complementary filter
   g_nFilterX = ComplementaryFilter(g_nFilterX, nAngleX, nRateX, QUADCTRL_SAMPLE_RATE);
   g_nFilterY = ComplementaryFilter(g_nFilterY, nAngleY, nRateY, QUADCTRL_SAMPLE_RATE);
   // return the results
   pSensor->nRollAngle  = g_nFilterX;
   pSensor->nPitchAngle = g_nFilterY;
   pSensor->nYawRate    = mpu.Gyro.z;
   return pSensor;
}
