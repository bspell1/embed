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
#define WIICHUK_SELECT_PIN          PIN_C3         // chuk chip select pin
#define WIICHUK_MESSAGE_SIZE        12             // protocol message size
//-------------------[        Module Variables         ]-------------------//
static const BYTE g_pbWiiChukInit1[] = { 0xF0, 0x55 };
static const BYTE g_pbWiiChukInit2[] = { 0xFB, 0x00 };
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
   sei();
   // initialize communication
   PinSetOutput(PIN_B0);
   I2cInit();
   SpiInit();
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_SS,
         .nCePin = PIN_B1
      }
   );
   Nrf24SetCrc(NRF24_CRC_NONE);
   Nrf24SetTXAddress("Wii00");
   Nrf24DisableAck();
   Nrf24PowerOn(NRF24_MODE_SEND);
   Nrf24SetRFDataRate(NRF24_RATE_1MBPS);
   Nrf24SetRFChannel(60);
   for ( ; ; )
   {
      BYTE buffer[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C };
      Nrf24Send(buffer, sizeof(buffer));
      PinToggle(PIN_B0);
      _delay_ms(1);
   }
   #if 0
   PinSetOutput(PIN_B0);
   PinSetLo(PIN_B0);
   #if 1
   WiiChukInit();
   #else
   PinSetOutput(PIN_C3);
   PinSetOutput(PIN_C4);
   PinSetOutput(PIN_C5);
   #endif
   for ( ; ; )
   {
      #if 1
      WiiChukRead();
      WiiChukSend();
      #else
      PinSetLo(PIN_C3);
      PinToggle(PIN_C4);
      _delay_ms(2000);
      PinToggle(PIN_C5);
      _delay_ms(2000);
      PinSetHi(PIN_C3);
      PinToggle(PIN_C4);
      _delay_ms(2000);
      PinToggle(PIN_C5);
      #endif
      PinToggle(PIN_B0);
      _delay_ms(1000);
   }
   #endif
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
   #if 0
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit1, sizeof(g_pbWiiChukInit1));
   _delay_ms(1);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit2, sizeof(g_pbWiiChukInit2));
   _delay_ms(1);
   #endif
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit1, sizeof(g_pbWiiChukInit1));
   _delay_ms(1);
   I2cSend(WIICHUK_ADDRESS, g_pbWiiChukInit2, sizeof(g_pbWiiChukInit2));
   _delay_ms(1);
}
//-----------< FUNCTION: WiiChukRead >----------------------------------------
// Purpose:    samples the state of the WiiChuk pair over I2C
//             stores the results in g_pbWiiChukMsg for transmission
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID WiiChukRead ()
{
   BYTE pbRegAddress[] = { 0x00 };
   // read chuk 0
   #if 0
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSend(
      WIICHUK_ADDRESS, 
      pbRegAddress, 
      sizeof(pbRegAddress)
   );
   _delay_ms(1);
   I2cRecv(
      WIICHUK_ADDRESS, 
      g_pbWiiChukMsg, 
      WIICHUK_MESSAGE_SIZE / 2
   );
   #endif
   // read chuk 1
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSend(
      WIICHUK_ADDRESS, 
      pbRegAddress, 
      sizeof(pbRegAddress)
   );
   _delay_ms(1);
   I2cRecv(
      WIICHUK_ADDRESS, 
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
