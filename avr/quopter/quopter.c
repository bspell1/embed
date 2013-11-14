//===========================================================================
// Module:  quopter.c
// Purpose: quadcopter program
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
#include "quopter.h"
#include "pid.h"
#include "tlc5940.h"
#include "i2cmast.h"
#include "mpu6050.h"
#include "quadctrl.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   PinSetOutput(PIN_D4);
   Tlc5940Init(
      &(TLC5940_CONFIG)
      {
         .nPinBlank = PIN_B1,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );
   I2cInit();
   sei();
   Mpu6050Init();
   Mpu6050Wake();
   Mpu6050DisableTemp();
   Mpu6050SetClockSource(MPU6050_CLOCK_PLLGYROX);
   Mpu6050SetLowPassFilter(MPU6050_DLPF_5HZ);
   QuadRotorInit(
      &(QUADROTOR_CONFIG)
      {
         .nTlc5940Module = 0,
         .nForeChannel   = 0,
         .nAftChannel    = 1,
         .nPortChannel   = 2,
         .nStarChannel   = 3
      }
   );
   // hover parameters
   F32 thrust = 0.2f;
   F32 pitch  = 0.0f;
   F32 roll   = 0.0f;
   F32 yaw    = 0.0f;
   PID pidRoll = 
   { 
      .nPGain   = 0.5f,
      .nIGain   = 0.2f,
      .nDGain   = 0.1f,
      .nControl = 0.0f
   };
   PID pidPitch = pidRoll;
   PID pidYaw = pidRoll;
   PidInit(&pidRoll);
   PidInit(&pidPitch);
   PidInit(&pidYaw);
   for ( ; ; ) {
      PinToggle(PIN_D4);
      // read the accelerometer, scale to 1g, and 
      // pass the readings  through the PID controllers
      MPU6050_VECTOR accel = Mpu6050ReadAccel();
      PidUpdate(&pidRoll, roll, accel.x * 2.0f);
      PidUpdate(&pidPitch, pitch, accel.y * 2.0f);
      PidUpdate(&pidYaw, yaw, 0.0f);
      // send the control signals to the rotors
      QuadRotorControl(
         thrust, 
         pidRoll.nControl,
         pidPitch.nControl,
         pidYaw.nControl
      );
      _delay_ms(1000);
   }
   return 0;
}
