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
#include "hcsr04.h"
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

   UartInit(&(UART_CONFIG) { 0 });

   HCSR04Init(
      (HCSR04_CONFIG[])
      {
         {
            .nTrigPin = PIN_ARDUINO_D5,
            .nEchoPin = PIN_ARDUINO_D6
         }
      }
   );

   for ( ; ; )
   {
      _delay_ms(1000);
      PinToggle(PIN_ARDUINO_LED);
      UI16 nReading = HCSR04Read(0);
      UartSendByte((BYTE)(nReading >> 8));
      UartSendByte((BYTE)(nReading & 0xFF));
   }

   return 0;
}
