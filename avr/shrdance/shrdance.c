//===========================================================================
// Module:  shrdance.c
// Purpose: dancing shift register program
//          toggles an alternating bit pattern across the chain of shift
//          registers every two seconds
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
#include "shrdance.h"
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
   // initialize the module
   PinSetOutput(PIN_ARDUINO_LED);
   PinSetLo(PIN_ARDUINO_LED);
   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_D6,
         .nLatchPin = PIN_D7,
         .nDataPin  = PIN_B0
      }
   );
   // initialize the contents of the shift register,
   // alternating zeroes and ones
   BYTE pbRegister[SHIFTREG_SIZE];
   memset(pbRegister, 0xAA, SHIFTREG_SIZE);
   // dance, monkey!
   for ( ; ; )
   {
      PinToggle(PIN_ARDUINO_LED);
      ShiftRegWrite(pbRegister);
      memset(pbRegister, ~pbRegister[0], SHIFTREG_SIZE);
      _delay_ms(2000);
   }
   return 0;
}
