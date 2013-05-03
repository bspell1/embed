//===========================================================================
// Module:  avrtest.c
// Purpose: AVR microprocessor test laboratory
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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
#include "avrtest.h"
#include "tlc5940.h"
#include "sx1509.h"
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
   cli();

   PIN_SET_OUTPUT(PIN_ARDUINO_LED);

   PIN_SET_LO(PIN_ARDUINO_LED);

   Tlc5940Init();

   HSX1509 h1509 = SX1509Init(0x3E);

   sei();

   #if 1
   SX1509SetDir(h1509, 0x0000);                       // set all pins to output
   for ( ; ; )
   {
      SX1509SetData(h1509, 0xFFFF);                   // set all pins high
      _delay_ms(15);
      SX1509SetData(h1509, 0x0000);                   // set all pins low
      _delay_ms(15);
   }
   #endif

   #if 0
   UI16 min = 90;
   UI16 max = 440;
   UI16 mul = 1;
   UI16 duty = min;
   I8 dir = 1;
   for ( ; ; )
   {
      Tlc5940SetDuty(0, duty);
      duty += dir * mul;
      if (duty < min)
         duty = min;
      else if (duty > max)
         duty = max;
      if (duty == min || duty == max)
      {
         dir *= -1;
         _delay_ms(2000);
      }
      _delay_ms(1);
   }
   #endif
   return 0;
}
