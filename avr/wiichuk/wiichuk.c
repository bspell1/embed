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
#include <avr/eeprom.h>
//-------------------[      Project Include Files      ]-------------------//
#include "wiichuk.h"
#include "i2cmast.h" 
#include "spimast.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
#define WIICHUK_ADDRESS_LENGTH      6              // NRF address buffer length
#define WIICHUK_ADDRESS_DEFAULT     "Wii00"        // default NRF address
#define WIICHUK_I2C_ADDRESS         0x52           // I2C address for Wii chuks
#define WIICHUK_SELECT_PIN          PIN_C3         // chuk chip select pin
#define WIICHUK_MESSAGE_SIZE        12             // protocol message size
//-------------------[        Module Variables         ]-------------------//
// NRF address SRAM/EEPROM
static CHAR       g_szAddress[WIICHUK_ADDRESS_LENGTH];
static CHAR EEMEM g_szEEAddress[WIICHUK_ADDRESS_LENGTH] = WIICHUK_ADDRESS_DEFAULT;
// Wii nunchuk initialization protocol data
static const BYTE g_pbWiiChukInit1[] = { 0xF0, 0x55 };
static const BYTE g_pbWiiChukInit2[] = { 0xFB, 0x00 };
// WiiChuk data transfer buffer
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
      WiiChukRead();
      WiiChukSend();
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
   // load the transceiver address from EEPROM
   eeprom_read_block(&g_szAddress, &g_szEEAddress, WIICHUK_ADDRESS_LENGTH);
   // initialize communication
   // . initialize I2C for the Wii nunchuks and SPI for the NRF24
   // . initialize the NRF24
   //   - SPI slave select pin on B2
   //   - chip enable on B1
   //   - 16-bit CRC
   //   - transciever address ffrom EEPROM
   //   - transmit mode with no acknowledgements
   I2cInit();
   SpiInit();
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_SS,
         .nCePin = PIN_B1
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   Nrf24SetTXAddress(g_szAddress);
   Nrf24DisableAck();
   Nrf24PowerOn(NRF24_MODE_SEND);
   // initialize chuk 0
   // . set the chip select pin low to trigger the chuk 0 PNP transistor
   // . send the first initialization command
   // . wait for 1ms, per the Wii nunchuk protocol
   // . send the second initialization command
   PinSetOutput(WIICHUK_SELECT_PIN);
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_I2C_ADDRESS, g_pbWiiChukInit1, sizeof(g_pbWiiChukInit1));
   _delay_ms(1);
   I2cSend(WIICHUK_I2C_ADDRESS, g_pbWiiChukInit2, sizeof(g_pbWiiChukInit2));
   _delay_ms(1);
   // initialize chuk 1
   // . set the chip select pin high to trigger the chuk 1 NPN transistor
   // . send the first initialization command
   // . wait for 1ms, per the Wii nunchuk protocol
   // . send the second initialization command
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSend(WIICHUK_I2C_ADDRESS, g_pbWiiChukInit1, sizeof(g_pbWiiChukInit1));
   _delay_ms(1);
   I2cSend(WIICHUK_I2C_ADDRESS, g_pbWiiChukInit2, sizeof(g_pbWiiChukInit2));
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
   // . set the chip select pin low to trigger the chuk 0 PNP transistor
   // . send the I2C read address (0)
   // . wait for 1ms, per the Wii nunchuk protocol
   // . receive the 6-byte data from the chuk
   // . note that send/recv cannot be combined into a single transaction
   PinSetLo(WIICHUK_SELECT_PIN);
   I2cSend(
      WIICHUK_I2C_ADDRESS, 
      pbRegAddress, 
      sizeof(pbRegAddress)
   );
   _delay_ms(1);
   I2cRecv(
      WIICHUK_I2C_ADDRESS, 
      g_pbWiiChukMsg, 
      WIICHUK_MESSAGE_SIZE / 2
   );
   // hold the select pin low while the I2C transaction completes
   _delay_us(10);
   // read chuk 1
   // . set the chip select pin high to trigger the chuk 1 NPN transistor
   // . send the I2C read address (0)
   // . wait for 1ms, per the Wii nunchuk protocol
   // . receive the 6-byte data from the chuk
   // . note that send/recv cannot be combined into a single transaction
   PinSetHi(WIICHUK_SELECT_PIN);
   I2cSend(
      WIICHUK_I2C_ADDRESS, 
      pbRegAddress, 
      sizeof(pbRegAddress)
   );
   _delay_ms(1);
   I2cRecv(
      WIICHUK_I2C_ADDRESS, 
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
