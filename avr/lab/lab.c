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

   TCCR1A = (1 << COM1A1);
   TCCR1B = (1 << WGM13) | AvrClk1Scale(1);
   ICR1 = 255;// AvrClkTop(1, 50000);
   OCR1A = 0.1f * ICR1;

   PinSetOutput(PIN_D4);
   PinSetOutput(PIN_B0);
   PinSetOutput(PIN_B1);

   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_D5,
         .nLatchPin = PIN_D6,
         .nDataPin  = PIN_D7
      }
   );

   static F32 nMsDelay = 1.1;

   static BYTE pbStages[4] = {
      0x99,    // 10011001b, 1010 stage
      0x96,    // 10010110b, 0110 stage
      0x66,    // 01100110b, 0101 stage
      0x69,    // 01101001b, 1001 stage
   };

   for ( ; ; )
   {
      for (UI16 j = 0; j < (UI16)100; j++)
      {
         for (UI8 i = 0; i < ARRAYLENGTH(pbStages); i++)
         {
            ShiftRegWrite8(0, pbStages[i]);
            _delay_ms(nMsDelay); 
         }
      }
      ShiftRegWrite8(0, 0);
      PinToggle(PIN_D4);
      _delay_ms(1000);
   }
   return 0;
}
