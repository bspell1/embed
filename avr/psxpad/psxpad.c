//===========================================================================
// Module:  psxpad.c
// Purpose: AVR Playstation control pad NRF24 interface
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
#include "psxpad.h"
#include "spimast.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
#define PSX_ADDRESS_LENGTH    6                 // NRF address buffer length
#define PSX_ADDRESS_DEFAULT   "Psx00"           // default NRF address
#define PSX_PIN_LED           PIN_D4            // LED heartbeat pin
#define PSX_PAD_PIN_SS        PIN_B0            // PSX pad slave select pin
#define PSX_MESSAGE_LENGTH    6                 // controller state buffer length
#define PSX_CLOCK_PERIOD_2    16                // PSX pad SPI clock period / 2, in us
#define PSX_NRF24_PIN_SS      PIN_SS            // NRF24 slave select pin
#define PSX_NRF24_PIN_CE      PIN_B1            // NRF24 CE pin
//-------------------[        Module Variables         ]-------------------//
// NRF address SRAM/EEPROM
static CHAR       g_szAddress[PSX_ADDRESS_LENGTH];
static CHAR EEMEM g_szEEAddress[PSX_ADDRESS_LENGTH] = PSX_ADDRESS_DEFAULT;
// PSX message buffer
static BYTE       g_pbMessage[PSX_MESSAGE_LENGTH];
//-------------------[        Module Prototypes        ]-------------------//
static VOID       PsxInit           ();
static BOOL       PsxRead           ();
static VOID       PsxSend           ();
static PBYTE      PsxSpiExchange    (PBYTE pbMessage, UI8 cbMessage);
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   PsxInit();
   for ( ; ; )
      if (PsxRead())
         PsxSend();
   return 0;
}
//-----------< FUNCTION: PsxInit >-------------------------------------------
// Purpose:    initializes the module
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID PsxInit ()
{
   sei();
   PinSetOutput(PSX_PIN_LED);
   // load the transceiver address from EEPROM
   eeprom_read_block(&g_szAddress, &g_szEEAddress, PSX_ADDRESS_LENGTH);
   // initialize communication
   // . initialize SPI hardware for the NRF24
   // . initialize the NRF24
   //   - SPI slave select pin on B2/SS
   //   - chip enable on B1
   //   - 16-bit CRC
   //   - transceiver address ffrom EEPROM
   //   - transmit mode with no acknowledgements
   SpiInit();
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PSX_NRF24_PIN_SS,
         .nCePin = PSX_NRF24_PIN_CE
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   Nrf24SetTXAddress(g_szAddress);
   Nrf24DisableAck();
   Nrf24SetPipeAutoAck(NRF24_PIPE0, FALSE);
   Nrf24PowerOn(NRF24_MODE_SEND);
   // initialize PSX pins
   PinSetHi(PSX_PAD_PIN_SS);
   PinSetOutput(PSX_PAD_PIN_SS);
   // enter PSX pad config mode
   PsxSpiExchange((BYTE[]) { 0x01, 0x43, 0x00, 0x01 }, 4);
   // switch the pad to analog mode
   PsxSpiExchange((BYTE[]) { 0x01, 0x44, 0x00, 0x01, 0x00 }, 5);
   // exit PSX pad config mode
   PsxSpiExchange((BYTE[]) { 0x01, 0x43, 0x00, 0x00 }, 4);
}
//-----------< FUNCTION: PsxRead >-------------------------------------------
// Purpose:    samples the PSX pad input state
// Parameters: none
// Returns:    true if the pad state was read successfully
//             false otherwise
//---------------------------------------------------------------------------
BOOL PsxRead ()
{
   // send command 0x42 to read the pad state, and exchange 9 bytes
   BYTE pbBuffer[PSX_MESSAGE_LENGTH + 3] = { 0x01, 0x42, 0x00, };
   PsxSpiExchange(pbBuffer, PSX_MESSAGE_LENGTH + 3);
   // verify the results and copy them to the output buffer
   if (pbBuffer[2] == 0x5A)
   {
      memcpy(g_pbMessage, pbBuffer + 3, PSX_MESSAGE_LENGTH);
      return TRUE;
   }
   return FALSE;
}
//-----------< FUNCTION: PsxSend >-------------------------------------------
// Purpose:    outputs the PSX pad state on the NRF24
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID PsxSend ()
{
   // send the PSX message
   Nrf24Send(g_pbMessage, PSX_MESSAGE_LENGTH);
   // toggle the heartbeat LED
   static UI16 g_nCounter = 0;
   if (g_nCounter++ == 1000)
   {
      g_nCounter = 0;
      PinToggle(PSX_PIN_LED);
   }
}
//-----------< FUNCTION: PsxSpiExchange >------------------------------------
// Purpose:    exchanges a message with the PSX pad over SPI
// Parameters: pbMessage - message buffer
//             cbMessage - size, in bytes, of the message to exchange
// Returns:    pbMessage
//---------------------------------------------------------------------------
PBYTE PsxSpiExchange (PBYTE pbMessage, UI8 cbMessage)
{
   // disable SPI hardware, since we are bit banging on the same pins
   SpiWait();
   SPCR &= ~(1 << SPE);
   // configure SPI pins for bit banging
   PinSetHi(PIN_SCK);
   PinSetOutput(PIN_SCK);
   PinSetLo(PIN_MOSI);
   PinSetOutput(PIN_MOSI);
   PinSetPullUp(PIN_MISO);
   // pull CS low with a half-period delay
   PinSetLo(PSX_PAD_PIN_SS);
   _delay_us(PSX_CLOCK_PERIOD_2);
   // exchange all requested data bytes
   for (UI8 i = 0; i < cbMessage; i++)
   {
      BYTE bSend = pbMessage[i];
      BYTE bRecv = 0;
      // exchange the current data byte, LSB first
      for (I8 j = 0; j < 8; j++)
      {
         // CPOL, so clock is low when active
         // CPHA, so set the output value when clock is low
         PinSetLo(PIN_SCK);
         PinSet(PIN_MOSI, (bSend >> j) & 0x1);
         _delay_us(PSX_CLOCK_PERIOD_2);
         // set the clock high and sample the input bit
         PinSetHi(PIN_SCK);
         _delay_us(PSX_CLOCK_PERIOD_2);
         bRecv |= (PinRead(PIN_MISO) << j);
      }
      pbMessage[i] = bRecv;
   }
   // reset CS to end the transaction
   PinSetHi(PSX_PAD_PIN_SS);
   _delay_us(PSX_CLOCK_PERIOD_2);
   // re-enable the SPI hardware and return the results
   SPCR |= (1 << SPE);
   return pbMessage;
}
