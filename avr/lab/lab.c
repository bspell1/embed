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
   TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
   TCCR0B = (1 << WGM02) | (1 << CS02) | (1 << CS00);
   PinSetOutput(PIN_D4);
   PinSetOutput(PIN_OC0A); // D6
   PinSetOutput(PIN_OC0B); // D5
   sei();

   for ( ; ; ) {
      OCR0A = 16;
      OCR0B = 19;
      _delay_ms(1000);
      OCR0A = 19;
      OCR0B = 16;
      _delay_ms(1000);
   }
#if 0
   PwmBangInit(
      &(PWMBANG_CONFIG)
      {
         .pPins = (UI8[PWMBANG_CHANNEL_COUNT])
         { 
            PIN_D5, PIN_D6, PIN_D7, PIN_B0 
         }
      }
   );
   for ( ; ; ) {
      PwmBangSetDutyF(0, 0.2f);
      PwmBangSetDutyF(1, 0.0f);
      PwmBangSetDutyF(2, 0.0f);
      _delay_ms(1000);
      PwmBangSetDutyF(0, 0.0f);
      PwmBangSetDutyF(1, 0.2f);
      PwmBangSetDutyF(2, 0.0f);
      _delay_ms(1000);
      PwmBangSetDutyF(0, 0.0f);
      PwmBangSetDutyF(1, 0.0f);
      PwmBangSetDutyF(2, 0.2f);
      _delay_ms(1000);
      PinToggle(PIN_D4);
   }
#endif
   return 0;
}
