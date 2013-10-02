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
#include "debug.h"
#include "uart.h"
#include "tlc5940.h"
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
   // initialize the module
   PinSetLo(PIN_ARDUINO_LED);
   Tlc5940Init(
      &(TLC5940_CONFIG) {
         .nPinBlank = PIN_ARDUINO_D7,
         .nPinSClk  = PIN_ARDUINO_D8,
         .nPinSIn   = PIN_ARDUINO_D9,
         .nPinXlat  = PIN_ARDUINO_D10,
         .nPinGSClk = PIN_ARDUINO_D6
      }
   );

   I16  min = 1;
   I16  max = 30;
   I16  cur = 0;
   BOOL dir = TRUE;
   for ( ; ; )
   {
      Tlc5940SetDuty(0, 15, cur);
      if (dir)
      {
         if (++cur == max)
            dir = FALSE;
      }
      else
      {
         if (--cur == min)
            dir = TRUE;
      }
      _delay_ms(10);
   }
   return 0;
}
