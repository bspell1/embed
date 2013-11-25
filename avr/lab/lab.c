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
   SpiInit();
   PinSetLo(PIN_D4);
   PinSetOutput(PIN_D4);
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_SS,
         .nCePin = PIN_C0
      }
   );
   // TX/RX config
   Nrf24SetCrc(NRF24_CRC_16BIT);
   Nrf24DisableAck();
   // TX config
   Nrf24SetTXAddress("Qop01");
   Nrf24SetPipeAutoAck(NRF24_PIPE0, FALSE);
   // RX config
   Nrf24SetPipeRXEnabled(NRF24_PIPE1, TRUE);
   Nrf24SetRXAddress(NRF24_PIPE1, "Wii00");
   Nrf24SetPayloadLength(NRF24_PIPE1, 12);
   Nrf24SetPipeAutoAck(NRF24_PIPE1, FALSE);
   // power up the transceiver
   Nrf24PowerOn(NRF24_MODE_RECV);
   for ( ; ; )
   {
      if (Nrf24ClearIrq(NRF24_IRQ_ALL) & NRF24_IRQ_RX_DR)
      {
         BYTE pbBuffer[12]; memzero(pbBuffer, sizeof(pbBuffer));
         Nrf24Recv(pbBuffer, sizeof(pbBuffer));
         Nrf24PowerOn(NRF24_MODE_SEND);
         Nrf24Send(pbBuffer, sizeof(pbBuffer));
         Nrf24PowerOn(NRF24_MODE_RECV);
         PinToggle(PIN_D4);
      }
      _delay_ms(50);
   }
   return 0;
}
