//===========================================================================
// Module:  steptest.c
// Purpose: AVR stepper motor test program
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
#include "steptest.h"
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
         .nClockPin = PIN_D2,
         .nLatchPin = PIN_D3,
         .nDataPin  = PIN_D4
      }
   );
   static const UI8 g_pStages[] = 
   {
      0xA,                                   // 1010, magnet 1/magnet 3 on
      0x6,                                   // 0110, magnet 2/magnet 3 on
      0x5,                                   // 0101, magnet 2/magnet 4 on
      0x9                                    // 1001, magnet 1/magnet 4 on
   };
   static const float g_pnDelay[] = 
   {
      2000,
      2000
   };
   static UI8   nMotors    = 2;
   static UI8   nSteps     = 50;
   static UI8   nRotations = 100;
   for ( ; ; )
   {
      PinToggle(PIN_ARDUINO_LED);

      for (UI8 i = 0; i < nMotors; i++)
      {
         for (UI8 j = 0; j < nRotations; j++)
         {
            for (UI8 i = 0; i < nMotors; i++)
            {
               for (UI8 k = 0; k < nSteps; k++)
               {
                  for (UI8 l = 0; l < 4; l++)
                  {
                     ShiftRegWrite4(i, g_pStages[l]);
                     _delay_us(g_pnDelay[i]);
                  }
               }
               ShiftRegWrite4(i, 0);
            }
         }
         _delay_ms(100);
      }
   }
   return 0;
}
