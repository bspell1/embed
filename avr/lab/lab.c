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
   PinSetOutput(PIN_D7);
   SpiInit();
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_SS,
         .nCePin = PIN_B1
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   Nrf24SetTXAddress("Lab00");
   Nrf24DisableAck();
   Nrf24SetPipeAutoAck(NRF24_PIPE0, FALSE);
   Nrf24PowerOn(NRF24_MODE_SEND);

   for ( ; ; )
   {
      PinToggle(PIN_D7);
      Nrf24Send((BYTE[]) { 0xC0, 0x0C, 0xC0, 0x0C, 0xC0, 0x0C }, 6);
      _delay_ms(500);
      PinToggle(PIN_D7);
      Nrf24Send((BYTE[]) { 0x0C, 0xC0, 0x0C, 0xC0, 0x0C, 0xC0 }, 6);
      _delay_ms(500);
   }
   return 0;
}
