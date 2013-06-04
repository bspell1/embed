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
#include "i2cmast.h"
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
   PinSetOutput(PIN_C3);

   UartInit(&(UART_CONFIG) { 0 });
   I2cInit();

   UartSendByte(0xC0);

   BYTE pbInit1[] = { 0xF0, 0x55 };
   BYTE pbInit2[] = { 0xFB, 0x00 };

   PinSetHi(PIN_C3);
   I2cSend(0x52, pbInit1, sizeof(pbInit1));
   _delay_ms(1);
   I2cSend(0x52, pbInit2, sizeof(pbInit2));
   _delay_ms(1);
   
   PinSetLo(PIN_C3);
   I2cSend(0x52, pbInit1, sizeof(pbInit1));
   _delay_ms(1);
   I2cSend(0x52, pbInit2, sizeof(pbInit2));
   _delay_ms(1);

   for ( ; ; )
   {
      BYTE pbMessage[6];

      PinSetHi(PIN_C3);
      I2cSend(0x52, (BYTE[]) { 0x00 }, 1);
      _delay_ms(1);
      I2cRecv(0x52, pbMessage, 6);
      UartSend(pbMessage, sizeof(pbMessage));

      PinSetLo(PIN_C3);
      I2cSend(0x52, (BYTE[]) { 0x00 }, 1);
      _delay_ms(1);
      I2cRecv(0x52, pbMessage, 6);
      UartSend(pbMessage, sizeof(pbMessage));

      _delay_ms(1000);
   }
   return 0;
}
