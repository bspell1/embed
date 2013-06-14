//===========================================================================
// Module:  avrlab.c
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
#include "avrlab.h"
#include "debug.h"
#include "uart.h"
#include "shiftreg.h"
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

   PinSetOutput(PIN_ARDUINO_LED);
   PinSetOutput(PIN_B0);

   UartInit(&(UART_CONFIG) { 0 });

   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nShiftClockPin = PIN_D2,
         .nStoreClockPin = PIN_D3,
         .nDataOutputPin = PIN_D4
      }
   );

   ShiftRegWrite8(0, 0xAA);
   ShiftRegWrite8(1, 0xAA);
   for ( ; ; )
   {
      ShiftRegWrite8(0, ~ShiftRegRead8(0));
      ShiftRegWrite8(1, ~ShiftRegRead8(1));
      _delay_ms(500);
      PinToggle(PIN_B0);
   }

   return 0;
}
