//===========================================================================
// Module:  wiichuk.c
// Purpose: Nintendo Wii nunchuk pair reader program entry point
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
#include "wiichuk.h"
#include "uart.h"
#include "i2cmast.h"
#include "spimast.h"
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

   PinSetLo(PIN_D4);
   PinSetOutput(PIN_D4);

   UartInit(&(UART_CONFIG) { 0 });
   I2cInit();
   SpiInit();

   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_SS,
         .nCePin = PIN_B1
      }
   );
   Nrf24SetTXAddress("Wii00");
   Nrf24DisableAck();
   Nrf24PowerOn(NRF24_MODE_SEND);

   for ( ; ; )
   {
      Nrf24Send(STR("Hello"), 5);
      PinToggle(PIN_D4);
      _delay_ms(1000);
   }

   return 0;
}
