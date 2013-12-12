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
#include "spimast.h"
#include "mpu6050.h"
#include "tlc5940.h"
#include "nrf24.h"
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
   PinSetOutput(PIN_D4);

   PinSetOutput(PIN_D6);
   PinSetOutput(PIN_D7);
   PinSetOutput(PIN_B0);

   UI16 delay = 50;
   UI8 phase1 = PIN_D6;
   UI8 phase2 = PIN_D7;
   UI8 phase3 = PIN_B0;

   for ( ; ; )
   {
      PinSet(phase1, BIT_HI); _delay_ms(delay);
      PinSet(phase3, BIT_LO); _delay_ms(delay);
      PinSet(phase2, BIT_HI); _delay_ms(delay);
      PinSet(phase1, BIT_LO); _delay_ms(delay);
      PinSet(phase3, BIT_HI); _delay_ms(delay);
      PinSet(phase2, BIT_LO); _delay_ms(delay);
   }
   return 0;
}
