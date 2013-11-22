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
#include "tlc5940.h"
#include "i2cmast.h"
#include "quadmpu.h"
#include "quadrotr.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
static void QuopterInit ();
static void QuopterRun  ();
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   QuopterInit();
   for ( ; ; )
      QuopterRun();
   return 0;
}
//-----------< FUNCTION: QuopterInit >---------------------------------------
// Purpose:    quopter initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
void QuopterInit ()
{
   // protocol initialization
   sei();
   I2cInit();
   // hardware initialization
   PinSetOutput(PIN_D4);
   PinSetHi(PIN_D4);
   Tlc5940Init(
      &(TLC5940_CONFIG) {
         .nPinBlank = PIN_B1,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );
   // module initialization
   QuadMpuInit(
      &(QUADMPU_CONFIG)
      {
      }
   );
   QuadRotorInit(
      &(QUADROTOR_CONFIG)
      {
         .nTlc5940Module = 0,
         .nBowChannel    = 0,
         .nSternChannel  = 1,
         .nPortChannel   = 2,
         .nStarChannel   = 3
      }
   );
   // start the first async sensor read
   QuadMpuBeginRead();
   PinSetLo(PIN_D4);
}
//-----------< FUNCTION: QuopterRun >----------------------------------------
// Purpose:    quopter loop execution
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
void QuopterRun  ()
{
   // toggle the LED to calibrate sample timings
   static UI16 g_nSamples  = 0;
   if (g_nSamples++ == 1000)
   {
      g_nSamples = 0;
      PinToggle(PIN_D4);
   }
   // retrieve the sensor readings and start the next async read
   QUADMPU_SENSOR mpu;
   QuadMpuEndRead(&mpu);
   QuadMpuBeginRead();
   // send the control signals to the rotors
   QuadRotorControl(
      &(QUADROTOR_CONTROL)
      {
         .nThrustInput = 0.5f,
         .nRollInput   = 0.0f,
         .nPitchInput  = 0.0f,
         .nYawInput    = 0.0f,
         .nRollSensor  = mpu.nRollAngle / M_PI_2,
         .nPitchSensor = mpu.nPitchAngle / M_PI_2,
         .nYawSensor   = mpu.nYawRate
      }
   );
}
