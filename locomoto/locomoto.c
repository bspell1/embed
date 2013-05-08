//===========================================================================
// Module:  locomoto.c
// Purpose: AVR microprocessor test laboratory
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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
#include "locomoto.h"
#include "uart.h"
#include "i2cmast.h"
#include "tlc5940.h"
#include "sx1509.h"
#include "stepmoto.h"
#include "proto.h"
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
   PinSetLo(PIN_ARDUINO_LED);

   UartInit();
   I2cInit();
   SX1509Init();
   Tlc5940Init(
      &(TLC5940_CONFIG)
      {
         .nPinBlank = PIN_D2,                      // arduino 2
         .nPinSClk  = PIN_D3,                      // arduino 3
         .nPinSIn   = PIN_D4,                      // arduino 4
         .nPinXlat  = PIN_D5,                      // arduino 5
         .nPinGSClk = PIN_OC0A                     // arduino 6
      }
   );
   StepMotorInit(
      (STEPMOTOR_CONFIG[STEPMOTO_COUNT])
      {
         { 
            .n1509Module = 0,                         // TLC5940 module 0
            .n1509Offset = 0                          // TLC5940 pins 0-3
         },
         { 
            .n1509Module = 0,                         // TLC5940 module 0
            .n1509Offset = 4                          // TLC5940 pins 4-7
         }
      }
   );
   ProtoInit();

   for ( ; ; )
   {
      _delay_ms(1000);
   }

   return 0;
}
