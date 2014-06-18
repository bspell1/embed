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
#define PIN_LED   PIN_D7
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
static VOID LabInit  ();
static VOID LabRun   ();
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   LabInit();
   for ( ; ; )
      LabRun();
   return 0;
}
//-----------< FUNCTION: LabInit >-------------------------------------------
// Purpose:    lab program initialiization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LabInit ()
{
   sei();

   PinSetOutput(PIN_LED);

   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_D2,
         .nLatchPin = PIN_D3,
         .nDataPin  = PIN_D4
      }
   );
}
//-----------< FUNCTION: LabRun >--------------------------------------------
// Purpose:    lab main loop
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LabRun ()
{
   static F32  nMsDelay = 2.1f;
   static UI16 nCycles  = 400;

   static BYTE pbStages[] = {
      /* stepper motor, dual bridge */
      0b01010101,    // 1010 stage
      0b01100110,    // 0110 stage
      0b10101010,    // 0101 stage
      0b10011001,    // 1001 stage
      /**/
      /* dc, pwm on shutdown, forward bridge 1, reverse bridge 2 
//      0b00000101,
      0b00001010,
      /**/
   };

   for ( ; ; )
   {
      for (UI16 j = 0; j < nCycles; j++)
      {
         for (UI8 i = 0; i < ARRAYLENGTH(pbStages); i++)
         {
//            ShiftRegWrite8(0, pbStages[i]);
            _delay_ms(nMsDelay); 
         }
      }
      ShiftRegWrite8(0, 0);
      PinToggle(PIN_LED);
      _delay_ms(1000);
   }
}
