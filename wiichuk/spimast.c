//===========================================================================
// Module:  spimast.c
// Purpose: AVR SPI master driver
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
//-------------------[      Project Include Files      ]-------------------//
#include "spimast.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
static UI8  g_nSsPin = PIN_INVALID;                   // slave select pin
static BYTE g_pbBuffer[SPI_BUFFER_SIZE];              // send/receive buffer
static BYTE g_cbBuffer = 0;                           // current buffer length
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SpiInit >-------------------------------------------
// Purpose:    SPI interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID SpiInit ()
{
   PinSetOutput(PIN_SCK);
   PinSetOutput(PIN_MOSI);
   PinSetOutput(PIN_SS);
   PinSetHi(PIN_SS);
   SPCR = (1 << SPE) | (1 << MSTR) |         // enable SPI master mode
          (SPI_LSB  << DORD) |               // set bit order
          (SPI_CPOL << CPOL) |               // set clock polarity
          (SPI_CPHA << CPHA);                // set clock phase
   // set clock frequency registers
#if (F_CPU / SPI_FREQUENCY == 2)             
   SPCR |= (0 << SPR1) | (0 << SPR0);
   SPSR |= (1 << SPI2X);
#elif (F_CPU / SPI_FREQUENCY == 4)
   SPCR |= (0 << SPR1) | (0 << SPR0);
#elif (F_CPU / SPI_FREQUENCY == 8)
   SPCR |= (0 << SPR1) | (1 << SPR0);
   SPSR |= (1 << SPI2X);
#elif (F_CPU / SPI_FREQUENCY == 16)
   SPCR |= (0 << SPR1) | (1 << SPR0);
#elif (F_CPU / SPI_FREQUENCY == 32)
   SPCR |= (1 << SPR1) | (0 << SPR0);
   SPSR |= (1 << SPI2X);
#elif (F_CPU / SPI_FREQUENCY == 64)
   SPCR |= (1 << SPR1) | (0 << SPR0);
#elif (F_CPU / SPI_FREQUENCY == 128)
   SPCR |= (1 << SPR1) | (1 << SPR0);
#else
#  error invalid SPI_FREQUENCY value
#endif   
}
//-----------< FUNCTION: SpiIsBusy >-----------------------------------------
// Purpose:    polls the SPI busy state
// Parameters: none
// Returns:    TRUE if there is an I/O in progress
//             FALSE otherwise
//---------------------------------------------------------------------------
BOOL SpiIsBusy ()
{
   // busy if the interrupt is enabled
   return RegGet(SPCR, SPIE);
}
//-----------< FUNCTION: SpiWait >-------------------------------------------
// Purpose:    waits for the SPI bus to become available
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID SpiWait ()
{
   while (SpiIsBusy())
      ;
}
//-----------< FUNCTION: SpiSend >-------------------------------------------
// Purpose:    sends a message on the SPI interface
// Parameters: nSsPin - slave select pin number (or PIN_INVALID for no SS)
//             pvSend - send message buffer
//             cbSend - number of bytes to send
// Returns:    none
//---------------------------------------------------------------------------
VOID SpiSend (UI8 nSsPin, PVOID pvSend, BSIZE cbSend)
{
   SpiSendRecv(nSsPin, pvSend, cbSend, NULL, 0);
}
//-----------< FUNCTION: SpiRecv >-------------------------------------------
// Purpose:    receives a message on the SPI interface
// Parameters: nSsPin - slave select pin number (or PIN_INVALID for no SS)
//             pvRecv - receive message buffer
//             cbRecv - maximum number of bytes to receive
// Returns:    none
//---------------------------------------------------------------------------
VOID SpiRecv (UI8 nSsPin, PVOID pvRecv, BSIZE cbRecv)
{
   SpiSendRecv(nSsPin, NULL, 0, pvRecv, cbRecv);
}
//-----------< FUNCTION: SpiSendRecv >---------------------------------------
// Purpose:    executes a combined send/receive transaction on the SPI bus
// Parameters: nSsPin - slave select pin number (or PIN_INVALID for no SS)
//             pvSend - send message buffer
//             cbSend - number of bytes to send
//             pvRecv - receive message buffer
//             cbRecv - number of bytes to receive
// Returns:    none
//---------------------------------------------------------------------------
VOID SpiSendRecv (
   UI8   nSsPin, 
   PVOID pvSend, 
   BSIZE cbSend,
   PVOID pvRecv, 
   BSIZE cbRecv
)
{
   // wait for and enable the interrupt to lock SPI
   SpiWait();
   RegSetHi(SPCR, SPIE);
   // set up SPI state and transfer the send buffer
   g_nSsPin = nSsPin;
   cbSend = MIN(cbSend, SPI_BUFFER_SIZE);
   cbRecv = MIN(cbRecv, SPI_BUFFER_SIZE);
   g_cbBuffer = MAX(cbSend, cbRecv);
   if (pvSend != NULL)
      memcpy(g_pbBuffer, pvSend, cbSend);
   if (cbRecv > cbSend)
      memzero(g_pbBuffer + cbSend, cbRecv - cbSend);
   // enable the slave and transfer the first byte
   if (g_nSsPin != PIN_INVALID)
      PinSetLo(g_nSsPin);
   SPDR = g_pbBuffer[0];
   // if receiving, wait for the results
   if (pvRecv != NULL)
   {
      SpiWait();
      memcpy(pvRecv, g_pbBuffer, cbRecv);
   }
}
//-----------< INTERRUPT: SPI_STC_vect >-------------------------------------
// Purpose:    responds to SPI transfer complete events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(SPI_STC_vect)
{
   static UI8 g_cbXfer = 0;
   // transfer the received byte to the receive buffer
   // send the next byte while more data remains
   g_pbBuffer[g_cbXfer++] = SPDR;
   if (g_cbXfer < g_cbBuffer)
      SPDR = g_pbBuffer[g_cbXfer];
   else
   {
      g_cbXfer = 0;
      g_cbBuffer = 0;
      // end the transaction on the slave
      if (g_nSsPin != PIN_INVALID)
         PinSetHi(g_nSsPin);
      // disable this interrupt to free up SPI
      RegSetLo(SPCR, SPIE);
   }
}
