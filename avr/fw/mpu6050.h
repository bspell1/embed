//===========================================================================
// Module:  mpu6050.h
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
#ifndef __MPU6050_H
#define __MPU6050_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// MODULE CONFIGURATION
//===========================================================================
#ifndef MPU6050_I2CADDR
#  define MPU6050_I2CADDR           MPU6050_ADDR_0
#endif
//===========================================================================
// MPU6050 STRUCTURES
//===========================================================================
// sensor vector
typedef union tagMpu6050Vector
{
   struct
   {
      F32   x;
      F32   y;
      F32   z;
   };
   F32 v[3];
} MPU6050_VECTOR, *PMPU6050_VECTOR;
// all sensors
typedef struct tagMpu6050Sensors
{
   MPU6050_VECTOR Accel;
   F32            Temp;
   MPU6050_VECTOR Gyro;
} MPU6050_SENSORS, *PMPU6050_SENSORS;
//===========================================================================
// MPU6050 CONFIGURATION VALUES
//===========================================================================
// I2C addresses
#define MPU6050_ADDR_0              0x68     // default address (AD0 = 0)
#define MPU6050_ADDR_1              0x69     // AD0 = 1
// axis values
#define MPU6050_AXIS_X              0x0      // X-axis for gyro/accel
#define MPU6050_AXIS_Y              0x1      // Y-axis for gyro/accel
#define MPU6050_AXIS_Z              0x2      // Z-axis for gyro/accel
// frame sync values
#define MPU6050_FRAMESYNC_DISABLED  0x0      // input disabled (default)
#define MPU6050_FRAMESYNC_TEMP      0x1      // output on temp sensor bit 0
#define MPU6050_FRAMESYNC_GYROX     0x2      // output on x-gyro sensor bit 0
#define MPU6050_FRAMESYNC_GYROY     0x3      // output on y-gyro sensor bit 0
#define MPU6050_FRAMESYNC_GYROZ     0x4      // output on z-gyro sensor bit 0
#define MPU6050_FRAMESYNC_ACCELX    0x5      // output on x-accel sensor bit 0
#define MPU6050_FRAMESYNC_ACCELY    0x6      // output on y-accel sensor bit 0
#define MPU6050_FRAMESYNC_ACCELZ    0x7      // output on z-accel sensor bit 0
// low-pass filter values
#define MPU6050_DLPF_260HZ          0x0      // 260Hz accel, 256Hz gyro filter (default)
#define MPU6050_DLPF_180HZ          0x1      // 184Hz accel, 188Hz gyro filter
#define MPU6050_DLPF_90HZ           0x2      // 94Hz accel, 98Hz gyro filter
#define MPU6050_DLPF_45HZ           0x3      // 44Hz accel, 42Hz gyro filter
#define MPU6050_DLPF_20HZ           0x4      // 21Hz accel, 20Hz gyro filter
#define MPU6050_DLPF_10HZ           0x5      // 10Hz accel, 10Hz gyro filter
#define MPU6050_DLPF_5HZ            0x6      // 5Hz accel, 5Hz gyro filter
// gyro scale values
#define MPU6050_GYRO_SCALE_250DPS   0x0      // +-250deg/sec (default)
#define MPU6050_GYRO_SCALE_500DPS   0x1      // +-500deg/sec
#define MPU6050_GYRO_SCALE_1000DPS  0x2      // +-1000deg/sec
#define MPU6050_GYRO_SCALE_2000DPS  0x3      // +-2000deg/sec
// accel scale values
#define MPU6050_ACCEL_SCALE_2G      0x0      // +-2g (default)
#define MPU6050_ACCEL_SCALE_4G      0x1      // +-4g
#define MPU6050_ACCEL_SCALE_8G      0x2      // +-8g
#define MPU6050_ACCEL_SCALE_16G     0x3      // +-16g
// clock sources
#define MPU6050_CLOCK_INTERNAL8MHZ  0x0      // internal clock (default)
#define MPU6050_CLOCK_PLLGYROX      0x1      // x-axis gyroscope clock
#define MPU6050_CLOCK_PLLGYROY      0x2      // y-axis gyroscope clock
#define MPU6050_CLOCK_PLLGYROZ      0x3      // z-axis gyroscope clock
#define MPU6050_CLOCK_EXTERNAL32KHZ 0x4      // external 32kHz clock
#define MPU6050_CLOCK_EXTERNAL19HZ  0x5      // external 19kHz clock
#define MPU6050_CLOCK_NONE          0x7      // stops the clock
//===========================================================================
// MODULE API
//===========================================================================
// module initialiization
VOID              Mpu6050Init                ();
// general configuration register
UI8               Mpu6050GetFrameSync        ();
VOID              Mpu6050SetFrameSync        (UI8 nFrameSync);
UI8               Mpu6050GetLowPassFilter    ();
VOID              Mpu6050SetLowPassFilter    (UI8 nFilter);
// gyro configuration register
BOOL              Mpu6050GetGyroSelfTest     (UI8 nAxis);
VOID              Mpu6050SetGyroSelfTest     (UI8 nAxis, BOOL fTest);
UI8               Mpu6050GetGyroScale        ();
VOID              Mpu6050SetGyroScale        (UI8 nScale);
// accel configuration register
BOOL              Mpu6050GetAccelSelfTest    (UI8 nAxis);
VOID              Mpu6050SetAccelSelfTest    (UI8 nAxis, BOOL fTest);
UI8               Mpu6050GetAccelScale       ();
VOID              Mpu6050SetAccelScale       (UI8 nScale);
// sensor registers
F32               Mpu6050ReadAccelAxis       (UI8 nAxis);
MPU6050_VECTOR    Mpu6050ReadAccel           ();
F32               Mpu6050ReadTempCelsius     ();
F32               Mpu6050ReadTempFahrenheit  ();
F32               Mpu6050ReadGyroAxis        (UI8 nAxis);
MPU6050_VECTOR    Mpu6050ReadGyro            ();
MPU6050_SENSORS   Mpu6050ReadSensors         ();
// power management registers
VOID              Mpu6050Reset               ();
BOOL              Mpu6050IsAsleep            ();
VOID              Mpu6050Sleep               ();
VOID              Mpu6050Wake                ();
BOOL              Mpu6050IsCycling           ();
VOID              Mpu6050SetCycling          (BOOL fCycling);
BOOL              Mpu6050IsTempDisabled      ();
VOID              Mpu6050SetTempDisabled     (BOOL fDisabled);
UI8               Mpu6050GetClockSource      ();
VOID              Mpu6050SetClockSource      (UI8 nSource);
// register helpers
inline F32 Mpu6050GetGyroSelfTestX ()
   { return Mpu6050GetGyroSelfTest(MPU6050_AXIS_X); }
inline F32 Mpu6050GetGyroSelfTestY ()
   { return Mpu6050GetGyroSelfTest(MPU6050_AXIS_Y); }
inline F32 Mpu6050GetGyroSelfTestZ ()
   { return Mpu6050GetGyroSelfTest(MPU6050_AXIS_Z); }
inline F32 Mpu6050GetAccelSelfTestX ()
   { return Mpu6050GetAccelSelfTest(MPU6050_AXIS_X); }
inline F32 Mpu6050GetAccelSelfTestY ()
   { return Mpu6050GetAccelSelfTest(MPU6050_AXIS_Y); }
inline F32 Mpu6050GetAccelSelfTestZ ()
   { return Mpu6050GetAccelSelfTest(MPU6050_AXIS_Z); }
inline F32 Mpu6050ReadAccelX ()
   { return Mpu6050ReadAccelAxis(MPU6050_AXIS_X); }
inline F32 Mpu6050ReadAccelY ()
   { return Mpu6050ReadAccelAxis(MPU6050_AXIS_Y); }
inline F32 Mpu6050ReadAccelZ ()
   { return Mpu6050ReadAccelAxis(MPU6050_AXIS_Z); }
inline F32 Mpu6050ReadGyroX ()
   { return Mpu6050ReadGyroAxis(MPU6050_AXIS_X); }
inline F32 Mpu6050ReadGyroY ()
   { return Mpu6050ReadGyroAxis(MPU6050_AXIS_Y); }
inline F32 Mpu6050ReadGyroZ ()
   { return Mpu6050ReadGyroAxis(MPU6050_AXIS_Z); }
inline BOOL Mpu6050IsAwake ()
   { return !Mpu6050IsAsleep(); }
inline BOOL Mpu6050IsTempEnabled ()
   { return !Mpu6050IsTempDisabled(); }
inline VOID Mpu6050DisableTemp ()
   { Mpu6050SetTempDisabled(TRUE); }
inline VOID Mpu6050EnableTemp ()
   { Mpu6050SetTempDisabled(FALSE); }
#endif // __MPU6050_H  
