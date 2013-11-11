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
   PinSetOutput(PIN_ARDUINO_LED);
   Tlc5940Init(
      &(TLC5940_CONFIG) {
         .nPinBlank = PIN_B1,
         .nPinSClk  = PIN_D7,
         .nPinSIn   = PIN_D5,
         .nPinXlat  = PIN_B0,
         .nPinGSClk = PIN_OC0A            // PIN_D6, greyscale clock
      }
   );

#if 0
   PinSetOutput(PIN_ARDUINO_D12);
   for ( ; ; ) {
      float duty = 2;
      PinSetHi(PIN_ARDUINO_D12);
      _delay_ms(duty);
      PinSetLo(PIN_ARDUINO_D12);
      _delay_ms(20 - duty);
      static volatile UI8 g_nCycle = 0;
      if (g_nCycle++ == 50) {
         g_nCycle = 0;
         PinToggle(PIN_ARDUINO_LED);
      }
   }
#endif
#if 0
   for ( ; ; ) { 
      Tlc5940SetDuty(0, 0, 4096 - 1.5 * (4096 / 20) - 100);
      _delay_ms(2000);
      PinToggle(PIN_ARDUINO_LED);
      Tlc5940SetDuty(0, 0, 4096 - (4096 / 20));
      _delay_ms(2000);
      PinToggle(PIN_ARDUINO_LED);
   }
#else
   I16  min  = 4096 - (1.5) * (4096 / 20);
   I16  max  = 4096 - (4096 / 20);
   I16  step = 10;
   //I16  min = (4096 / 20);
   //I16  max = 2 * (4096 / 20);
   //I16  min  = 0;
   //I16  max  = 4095;
   //I16  step = 100;
   I16  cur  = min;
   BOOL dir  = TRUE;
   for ( ; ; )
   {
      Tlc5940SetDuty(0, 0, cur);
      if (dir)
      {
         if ((cur += step) > max)
         {
            cur = max;
            dir = FALSE;
            _delay_ms(1000);
            PinToggle(PIN_ARDUINO_LED);
         }
      }
      else
      {
         if ((cur -= step) < min)
         {
            cur = min;
            dir = TRUE;
            _delay_ms(1000);
            PinToggle(PIN_ARDUINO_LED);
         }
      }
      _delay_ms(100);
   }
#endif
   return 0;
}
