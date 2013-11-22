//===========================================================================
// Module:  lab.c
// Purpose: AVR test program
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
#include "lab.h"
#include "uart.h"
#include "i2cmast.h"
#include "mpu6050.h"
#include "tlc5940.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
static void SetDuty (F32 nDuty)
{
   /*
#define PWM_MIN (F32)1600
#define PWM_MAX (F32)2400
   Tlc5940SetDuty(0, 0, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 1, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 2, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 3, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   */
   /*
   UI16 PWM_MIN = 4096 / 20;
   UI16 PWM_MAX = 2 * PWM_MIN;
   Tlc5940SetDuty(0, 0, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 1, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 2, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   Tlc5940SetDuty(0, 3, 4095 - Map(nDuty, 0.0f, 1.0f, PWM_MIN, PWM_MAX));
   */
   Tlc5940SetDuty(0, 0, 4095 - (nDuty * 800 + 1600));
   Tlc5940SetDuty(0, 1, 4095 - (nDuty * 800 + 1600));
   Tlc5940SetDuty(0, 2, 4095 - (nDuty * 800 + 1600));
   Tlc5940SetDuty(0, 3, 4095 - (nDuty * 800 + 1600));
}
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   sei();
   PinSetLo(PIN_D4);
   PinSetOutput(PIN_D4);
   Tlc5940Init(
      &(TLC5940_CONFIG) {
         .nPinBlank = PIN_B1,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );
   _delay_ms(2000);
   PinSetHi(PIN_D4);
   SetDuty(2.0f);
   _delay_ms(2000);
   PinSetLo(PIN_D4);
   SetDuty(0.0f);
   _delay_ms(2000);
   for ( ; ; )
   {
      SetDuty(0.4);
      _delay_ms(5);
      PinToggle(PIN_D4);
      SetDuty(0.2);
      _delay_ms(5);
      PinToggle(PIN_D4);
   }
   return 0;
}
