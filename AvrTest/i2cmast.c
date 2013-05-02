//===========================================================================
// Module:  i2cmast.c
// Purpose: AVR I2C master driver
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
#include "i2cmast.h"
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// I2C DATA PROTOCOL
//===========================================================================
#define ADDR_READ_BIT               0
//===========================================================================
// I2C STATUS CODES
//===========================================================================
// general master status codes                      
#define STATUS_START                0x08  // START has been transmitted  
#define STATUS_REP_START            0x10  // repeated START has been transmitted
#define STATUS_ARB_LOST             0x38  // arbitration lost
// master transmitter status codes                    
#define STATUS_MTX_ADR_ACK          0x18  // SLA+W has been tramsmitted and ACK received
#define STATUS_MTX_ADR_NACK         0x20  // SLA+W has been tramsmitted and NACK received 
#define STATUS_MTX_DATA_ACK         0x28  // data byte has been tramsmitted and ACK received
#define STATUS_MTX_DATA_NACK        0x30  // data byte has been tramsmitted and NACK received 
// master receiver status codes  
#define STATUS_MRX_ADR_ACK          0x40  // SLA+R has been tramsmitted and ACK received
#define STATUS_MRX_ADR_NACK         0x48  // SLA+R has been tramsmitted and NACK received
#define STATUS_MRX_DATA_ACK         0x50  // data byte has been received and ACK tramsmitted
#define STATUS_MRX_DATA_NACK        0x58  // data byte has been received and NACK tramsmitted
// miscellaneous status codes
#define STATUS_NO_STATUS            0xF8  // no relevant state information available; TWINT = “0”
#define STATUS_BUS_ERROR            0x00  // bus error due to an illegal START or STOP condition
//===========================================================================
// I2C CONTROL CODES
//===========================================================================
#define CONTROL_BASE \
   (1<<TWEN)
#define CONTROL_INIT \
   CONTROL_BASE| \
   (0<<TWIE)|(0<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
#define CONTROL_START \
   CONTROL_BASE| \
   (1<<TWIE)|(1<<TWINT)| \
   (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO)
#define CONTROL_STOP \
   CONTROL_BASE|\
   (0<<TWIE)|(1<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO)
#define CONTROL_ABORT \
   CONTROL_BASE| \
   (0<<TWIE)|(1<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
#define CONTROL_SEND_DATA \
   CONTROL_BASE| \
   (1<<TWIE)|(1<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
#define CONTROL_SEND_ACK \
   CONTROL_BASE| \
   (1<<TWIE)|(1<<TWINT)| \
   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
#define CONTROL_SEND_NACK \
   CONTROL_BASE| \
   (1<<TWIE)|(1<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
//-------------------[        Module Variables         ]-------------------//
static I2C_CALLBACK  g_pCallback = NULL;
static BYTE          g_bAddrByte = 0;
static PBYTE         g_pbMessage = NULL;
static BSIZE         g_cbMessage = 0;
static BOOL          g_bXferOk   = FALSE;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: I2cInit >-------------------------------------------
// Purpose:    I2C interface initialization
// Parameters: pCallback - I/O completion callback
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cInit (I2C_CALLBACK pCallback)
{
   g_pCallback = pCallback;
   // initialize I2C registers
   TWSR = 0;                                 // prescale the I2C clock at 0, clear status
   TWBR = (F_CPU / 100000 - 16) / 2;         // set to 100kHz bit rate
   TWDR = 0xFF;                              // initialize data to SDA clear
   TWCR = CONTROL_INIT;
}
//-----------< FUNCTION: I2cIsBusy >-----------------------------------------
// Purpose:    polls the I2C busy state
// Parameters: none
// Returns:    TRUE if there is an I/O in progress
//             FALSE otherwise
//---------------------------------------------------------------------------
BOOL I2cIsBusy ()
{
   // busy if the interrupt is enabled
   return TWCR & (1<<TWIE);
}
//-----------< FUNCTION: I2cSend >-------------------------------------------
// Purpose:    sends a message on the I2C interface
// Parameters: nSlaveAddr - address of the slave to send to
//             pvMessage  - message buffer
//             cbMessage  - number of bytes to send
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cSend (BYTE nSlaveAddr, PVOID pvMessage, BSIZE cbMessage)
{
   while (I2cIsBusy()) ;
   // set up I2C state
   g_bAddrByte = nSlaveAddr & ~BIT_MASK(ADDR_READ_BIT);
   g_pbMessage = (PBYTE)pvMessage;
   g_cbMessage = cbMessage;
   g_bXferOk    = FALSE;
   // start the data transfer
   TWCR = CONTROL_START;
}
//-----------< FUNCTION: I2cReceive >----------------------------------------
// Purpose:    receives a message on the I2C interface
// Parameters: nSlaveAddr - address of the slave to receive from
//             pvMessage  - message buffer
//             cbBuffer   - maximum number of bytes to receive
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cReceive (UI8 nSlaveAddr, PVOID pvMessage, BSIZE cbBuffer)
{
   while (I2cIsBusy()) ;
   // set up I2C state
   g_bAddrByte = nSlaveAddr | BIT_MASK(ADDR_READ_BIT);
   g_pbMessage = (PBYTE)pvMessage;
   g_cbMessage = cbBuffer;
   g_bXferOk    = FALSE;
   // start the data transfer
   TWCR = CONTROL_START;
}
//-----------< INTERRUPT: TWI_vect >-----------------------------------------
// Purpose:    responds to TWI events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TWI_vect)
{
   static UI8 nMsgIdx = 0;
   switch (TWSR)
   {
      case STATUS_START:                           // START transmitted
      case STATUS_REP_START:                       // re-START transmitted
         nMsgIdx = 0;
         TWDR = g_bAddrByte;
         TWCR = CONTROL_SEND_DATA;
         break;
      case STATUS_ARB_LOST:                        // arbitration lost, restart
         TWCR = CONTROL_START;
         break;
      case STATUS_MTX_ADR_ACK:                     // address byte transmitted
      case STATUS_MTX_DATA_ACK:                    // data byte transmitted
         PIN_SET_HI(PIN_ARDUINO_LED); //TODO: remove
         if (nMsgIdx < g_cbMessage)
         {
            TWDR = g_pbMessage[nMsgIdx++];
            TWCR = CONTROL_SEND_DATA;
         }
         else
         {
            g_bXferOk = TRUE;
            TWCR = CONTROL_STOP;
         }
         break;
      case STATUS_MRX_ADR_ACK:                     // address byte transmitted
         TWCR = CONTROL_SEND_ACK;
         break;
      case STATUS_MRX_DATA_ACK:                    // data byte received
         g_pbMessage[nMsgIdx++] = TWDR;
         if (nMsgIdx < g_cbMessage - 1)
            TWCR = CONTROL_SEND_ACK;
         else
            TWCR = CONTROL_SEND_NACK;
         break;
      case STATUS_MRX_DATA_NACK:                   // NACK transmitted
         g_pbMessage[nMsgIdx++] = TWDR;
         g_cbMessage = nMsgIdx;
         g_bXferOk = TRUE;
         TWCR = CONTROL_STOP;
         break;
      case STATUS_MRX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_MTX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_MTX_DATA_NACK:                   // error - NACK was received after data
      case STATUS_BUS_ERROR:                       // general error
      default:                                     // unknown error
         g_cbMessage = nMsgIdx;
         TWCR = CONTROL_ABORT;
         break;
   }
}
