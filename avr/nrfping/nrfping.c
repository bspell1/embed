//===========================================================================
// Module:  nrfping.c
// Purpose: NRF24 ping program
//
// Copyright © 2014
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
#include "nrfping.h"
#include "spimast.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
// AVR pin configuration   
#define PIN_NRF24_SS          PIN_SS
#define PIN_NRF24_CE          PIN_B1
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
static VOID PingInit ();
static VOID PingRun  ();
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   PingInit();
   for ( ; ; )
      PingRun();
   return 0;
}
//-----------< FUNCTION: PingInit >-------------------------------------------
// Purpose:    ping program initialiization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID PingInit ()
{
   // protocol initialization
   sei();
   SpiInit();
   // hardware initialization
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_NRF24_SS,
         .nCePin = PIN_NRF24_CE
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   Nrf24DisableAck();
   Nrf24SetTXAddress("Nrf00");
   Nrf24SetPipeAutoAck(NRF24_PIPE0, FALSE);
   Nrf24PowerOn(NRF24_MODE_SEND);
}
//-----------< FUNCTION: PingRun >--------------------------------------------
// Purpose:    ping main loop
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID PingRun ()
{
   BYTE pbMessage[] = { 0x60, 0x0D, 0xF0, 0x0D };
   Nrf24Send(pbMessage, sizeof(pbMessage));
   _delay_ms(20);
}
