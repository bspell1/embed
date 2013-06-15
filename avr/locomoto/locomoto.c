//===========================================================================
// Module:  locomoto.c
// Purpose: LocoMoto motor driver program entry point
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
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
#include "locomoto.h"
#include "stepmoto.h"
#include "proto.h"
#include "uart.h"
#include "tlc5940.h"
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

   PinSetOutput(PIN_B0);
   PinSetLo(PIN_B0);

   UartInit(
      &(UART_CONFIG)
      {
         .pfnOnSend = NULL,
         .pfnOnRecv = ProtoRecvByte
      }
   );
   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_D2,
         .nLatchPin = PIN_D3,
         .nDataPin  = PIN_D4
      }
   );
   Tlc5940Init(
      &(TLC5940_CONFIG)
      {
         .nPinBlank = PIN_D2,
         .nPinSClk  = PIN_D3,
         .nPinSIn   = PIN_D4,
         .nPinXlat  = PIN_D5,
         .nPinGSClk = PIN_OC0A
      }
   );
   StepMotorInit(
      (STEPMOTOR_CONFIG[STEPMOTO_COUNT])
      {
         { .nSRNibble = 0 },
         { .nSRNibble = 1 },
      }
   );
   ProtoInit();

   for ( ; ; )
   {
      _delay_ms(1000);
   }

   return 0;
}
