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
#include "pwmbang.h"
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
   PinSetHi(PIN_D4);
   Tlc5940SetDuty(0, 0, (4096 - 2 * (4096 / 20)));
   Tlc5940SetDuty(0, 1, (4096 - 2 * (4096 / 20)));
   Tlc5940SetDuty(0, 2, (4096 - 2 * (4096 / 20)));
   Tlc5940SetDuty(0, 3, (4096 - 2 * (4096 / 20)));
   _delay_ms(2000);
   PinSetLo(PIN_D4);
   Tlc5940SetDuty(0, 0, (4096 - 1 * (4096 / 20)));
   Tlc5940SetDuty(0, 1, (4096 - 1 * (4096 / 20)));
   Tlc5940SetDuty(0, 2, (4096 - 1 * (4096 / 20)));
   Tlc5940SetDuty(0, 3, (4096 - 1 * (4096 / 20)));
   _delay_ms(2000);
   for ( ; ; )
   {
      Tlc5940SetDuty(0, 0, (4096 - 1.5 * (4096 / 20)));
      Tlc5940SetDuty(0, 1, (4096 - 1.5 * (4096 / 20)));
      Tlc5940SetDuty(0, 2, (4096 - 1.5 * (4096 / 20)));
      Tlc5940SetDuty(0, 3, (4096 - 1.5 * (4096 / 20)));
      _delay_ms(1000);
      PinToggle(PIN_D4);
      Tlc5940SetDuty(0, 0, (4096 - 1 * (4096 / 20)));
      Tlc5940SetDuty(0, 1, (4096 - 1 * (4096 / 20)));
      Tlc5940SetDuty(0, 2, (4096 - 1 * (4096 / 20)));
      Tlc5940SetDuty(0, 3, (4096 - 1 * (4096 / 20)));
      _delay_ms(1000);
      PinToggle(PIN_D4);
   }
   return 0;
}
