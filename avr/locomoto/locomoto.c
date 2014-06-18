//===========================================================================
// Module:  locomoto.c
// Purpose: LocoMoto motor driver program entry point
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
#include "locomoto.h"
#include "spimast.h"
#include "shiftreg.h"
#include "nrf24.h"
#include "stepper.h"
#include "locopsx.h"
//-------------------[       Module Definitions        ]-------------------//
// locomoto configuration
#define THROTTLE_ABSMIN       ((F32)0.15f)   // desensitize PSX joystick
#define THROTTLE_ABSMAX       ((F32)1.0f)    // maximum absolute throttle
#define DELAY_MIN             11             // 1.1ms at 50 steps/r => 273 RPM
#define DELAY_MAX             50             // 5.0ms at 50 steps/r => 60 RPM
#define MOTOR_LEFT            0              // left motor shift register
#define MOTOR_RIGHT           1              // right motor shift register
// AVR pin configuration   
#define PIN_LED               PIN_D7
#define PIN_SHIFTREG_CLOCK    PIN_D2
#define PIN_SHIFTREG_LATCH    PIN_D3
#define PIN_SHIFTREG_DATA     PIN_D4
#define PIN_NRF24_SS          PIN_B1
#define PIN_NRF24_CE          PIN_B0
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
static VOID LocoMotoInit      ();
static VOID LocoMotoRun       ();
static VOID LocoMotoRunMotor  (UI8 nMotor, F32 nThrottle);
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   LocoMotoInit();
   for ( ; ; )
      LocoMotoRun();
   return 0;
}
//-----------< FUNCTION: LocoMotoInit >--------------------------------------
// Purpose:    locomoto module initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoInit ()
{
   // protocol initialization
   sei();
   SpiInit();
   // hardware initialization
   PinSetOutput(PIN_LED);
   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_SHIFTREG_CLOCK,
         .nLatchPin = PIN_SHIFTREG_LATCH,
         .nDataPin  = PIN_SHIFTREG_DATA
      }
   );
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_NRF24_SS,
         .nCePin = PIN_NRF24_CE
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   // module initialization
   StepMotorInit(
      (STEPMOTOR_CONFIG[STEPPER_COUNT])
      {
         { .nShiftReg = 0 },
         { .nShiftReg = 1 },
      }
   );
   LocoPsxInit(
      &(LOCOPSX_CONFIG)
      {
         .pszAddress = LOCOPSX_ADDRESS,
         .nPipe      = 1
      }
   );
}
//-----------< FUNCTION: LocoMotoRun >---------------------------------------
// Purpose:    locomoto main loop
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoRun ()
{
   static UI8 g_nCounter = 0;
   // read the current PSX pad state
   LOCOPSX_INPUT psx;
   if (LocoPsxEndRead(&psx) == NULL) 
   {
      PinSetLo(PIN_LED);
      LocoMotoRunMotor(MOTOR_LEFT, 0);
      LocoMotoRunMotor(MOTOR_RIGHT, 0);
   }
   else
   {
      // flash the LED if receiving input
      if (g_nCounter == 0)
         PinToggle(PIN_LED);
      // set the throttle on each motor
      LocoMotoRunMotor(MOTOR_LEFT, -psx.nLY);
      LocoMotoRunMotor(MOTOR_RIGHT, psx.nRY);
   }
   LocoPsxBeginRead();
   g_nCounter++;
   _delay_ms(2);
}
//-----------< FUNCTION: LocoMotoRunMotor >----------------------------------
// Purpose:    runs a stepper motor
// Parameters: nMotor    - motor to run
//             nThrottle - motor speed
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoRunMotor  (UI8 nMotor, F32 nThrottle)
{
   if (fabs(nThrottle) < THROTTLE_ABSMIN)
      StepMotorStop(nMotor);
   else
      StepMotorRun(
         nMotor, 
         Map(
            fabs(nThrottle), 
            THROTTLE_ABSMIN,
            THROTTLE_ABSMAX,
            DELAY_MAX, 
            DELAY_MIN
         ),
         nThrottle > 0 ? STEPMOTOR_FORWARD : STEPMOTOR_REVERSE
      );
}
