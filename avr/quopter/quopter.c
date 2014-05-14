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
#include "spimast.h"
#include "nrf24.h"
#include "quadpsx.h"
#include "quadmpu.h"
#include "quadrotr.h"
#include "quadbay.h"
#include "quadtel.h"
//-------------------[       Module Definitions        ]-------------------//
#define QUOPTER_ROLL_BIAS  0.05
//-------------------[        Module Variables         ]-------------------//
static QUADROTOR_CONTROL   g_Control;
static BOOL                g_bBayOpen = FALSE;
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
   // global initialization
   memzero(&g_Control, sizeof(g_Control));
   g_Control.nThrustInput = 0.0f;
   // protocol initialization
   sei();
   I2cInit();
   SpiInit();
   // hardware initialization
   PinSetOutput(PIN_D4);
   PinSetHi(PIN_D4);
   Tlc5940Init(
      &(TLC5940_CONFIG) {
         .nPinBlank = PIN_D3,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_C1,
         .nCePin = PIN_C0
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   // module initialization
   QuadMpuInit(
      &(QUADMPU_CONFIG)
      {
      }
   );
   QuadPsxInit(
      &(QUADPSX_CONFIG)
      {
         .pszAddress = QUADPSX_ADDRESS,
         .nPipe      = 1
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
   QuadBayInit(
      &(QUADBAY_CONFIG)
      {
      }
   );
   QuadTelInit(
      &(QUADTEL_CONFIG)
      {
         .pszAddress = "Qop01"
      }
   );
   g_Control.nThrustInput = 0.0f;
   PinSetLo(PIN_D4);
}
//-----------< FUNCTION: QuopterRun >----------------------------------------
// Purpose:    quopter loop execution
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
void QuopterRun  ()
{
   static UI8 g_nCounter = 0;
   // start the next sensor/input reading
   QuadMpuBeginRead();
   QuadPsxBeginRead();
   // send the control signals to the rotors and bomb bay
   QuadRotorControl(&g_Control);
   QuadBayControl(g_bBayOpen);
   // retrieve the sensor readings
   QUADMPU_SENSOR mpu;
   QuadMpuEndRead(&mpu);
   mpu.nRollAngle += QUOPTER_ROLL_BIAS;
   g_Control.nRollSensor  = mpu.nRollAngle;
   g_Control.nPitchSensor = mpu.nPitchAngle;
   g_Control.nYawSensor   = mpu.nYawRate;
   // retrieve the input readings
   QUADPSX_INPUT psx;
   if (QuadPsxEndRead(&psx) == NULL)
      PinSetLo(PIN_D4);
   else
   {
      // apply inputs to rotor/bomb bay controls
      g_Control.nThrustInput += psx.nLY * 0.2f * QUADMPU_SAMPLE_TIME; // max 10%/sec
      g_Control.nRollInput    = -psx.nRX * M_PI / 18.0f;              // max 10deg
      g_Control.nPitchInput   = psx.nRY * M_PI / 18.0f;               // max 10deg
      g_bBayOpen              = psx.bR1;
      if (psx.bL2)
         g_Control.nThrustInput = 0.0f;
      else if (psx.bR2)
         g_Control.nThrustInput = 0.5f;
      if (g_nCounter == 0)
         PinToggle(PIN_D4);
   }
   // broadcast telemetrics
   QuadTelSend(
      &(QUADTEL_DATA)
      {
         .nRollAngle      = mpu.nRollAngle / M_PI * 180,
         .nPitchAngle     = mpu.nPitchAngle / M_PI * 180,
         .nYawRate        = mpu.nYawRate * 250,
         .nThrustInput    = g_Control.nThrustInput * 100,
         .nRollInput      = g_Control.nRollInput / M_PI * 180,
         .nPitchInput     = g_Control.nPitchInput / M_PI * 180,
         .nYawInput       = g_Control.nYawInput * 10,
         .nBowRotor       = g_Control.nBowRotor,
         .nSternRotor     = g_Control.nSternRotor,
         .nPortRotor      = g_Control.nPortRotor,
         .nStarboardRotor = g_Control.nStarboardRotor,
         .nCounter        = g_nCounter
      }
   );
   g_nCounter++;
}
