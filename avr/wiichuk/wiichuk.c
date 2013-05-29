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
//-------------------[      Project Include Files      ]-------------------//
#include "wiichuk.h"
#include "i2cmast.h" 
#include "spimast.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
#define WIICHUK_ADDRESS             0x52           // I2C address for Wii chuks
#define WIICHUK_SELECT_PIN          PC3            // chuk chip select pin
#define WIICHUK_MESSAGE_SIZE        12             // protocol message size
//-------------------[        Module Variables         ]-------------------//
static const BYTE g_pbWiiChukInit[] = { 0x40, 0x00 };
static BYTE       g_pbWiiChukMsg[WIICHUK_MESSAGE_SIZE];
//-------------------[        Module Prototypes        ]-------------------//
static VOID WiiChukInit ();
static VOID WiiChukRead ();
static VOID WiiChukSend ();
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   WiiChukInit();
   for ( ; ; )
   {
      Nrf24Send(STR("Hello"), 5);
      _delay_ms(1000);
      WiiChukRead();
      WiiChukSend();
      _delay_ms(1000);
   }

   return 0;
}
//-----------< FUNCTION: WiiChukInit >---------------------------------------
// Purpose:    initializes the WiiChuk pair over I2C
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID WiiChukInit ()
{
   sei();
   // initialize communication
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
   // initialize the Wii nunchuks
   PinSetOutput(WIICHUK_SELECT_PIN);
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit, sizeof(g_pbWiiChukInit));
   I2cWait();
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit, sizeof(g_pbWiiChukInit));
   I2cWait();
}
//-----------< FUNCTION: WiiChukRead >----------------------------------------
// Purpose:    samples the state of the WiiChuk pair over I2C
//             stores the results in g_pbWiiChukMsg for transmission
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID WiiChukRead ()
{
   BYTE pbAddress[] = { 0x00 };
   // read chuk 0
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSendRecv(
      WIICHUK_ADDRESS, 
      pbAddress, 
      sizeof(pbAddress),
      g_pbWiiChukMsg, 
      WIICHUK_MESSAGE_SIZE / 2
   );
   // read chuk 1
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSendRecv(
      WIICHUK_ADDRESS, 
      pbAddress, 
      sizeof(pbAddress),
      g_pbWiiChukMsg + WIICHUK_MESSAGE_SIZE / 2, 
      WIICHUK_MESSAGE_SIZE / 2
   );
}
//-----------< FUNCTION: WiiChukSend >----------------------------------------
// Purpose:    sends the current WiiChuk message out over the NRF24
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID WiiChukSend ()
{
   Nrf24Send(g_pbWiiChukMsg, WIICHUK_MESSAGE_SIZE);
}
