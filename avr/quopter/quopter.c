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
#include "quadctrl.h"
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
   Tlc5940Init(
      &(TLC5940_CONFIG)
      {
         .nPinBlank = PIN_B1,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );
   QuadRotorInit(
      &(QUADROTOR_CONFIG)
      {
         .nTlc5940Module = 0,
         .nForeChannel   = 0,
         .nAftChannel    = 1,
         .nPortChannel   = 2,
         .nStarChannel   = 3
      }
   );
   sei();
   F32  min = 0.0f;
   F32  max = 0.5f;
   F32  cur = min;
   F32  inc = 0.05f;
   BOOL dir = TRUE;
   for ( ; ; ) {
      QuadRotorControl(cur, 0, 0, 0);
      if (dir)
      {
         if ((cur += inc) > max)
         {
            cur = max;
            dir = FALSE;
            _delay_ms(1000);
            PinToggle(PIN_D4);
         }
      }
      else
      {
         if ((cur -= inc) < min)
         {
            cur = min;
            dir = TRUE;
            _delay_ms(1000);
            PinToggle(PIN_D4);
         }
      }
      _delay_ms(500);
   }
   return 0;
}
