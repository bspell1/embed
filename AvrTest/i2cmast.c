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
   TWCR = (1<<TWEN)|                         // enable I2C
          (0<<TWIE)|(0<<TWINT)|              // disable interrupts (not busy)
          (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);   // clear signal requests
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
   g_bAddrByte = nSlaveAddr & (1<<ADDR_READ_BIT);
   g_pbMessage = (PBYTE)pvMessage;
   g_cbMessage = cbMessage;
   g_bXferOk    = FALSE;
   // start the data transfer
   TWCR = (1<<TWEN)|                         // I2C enabled
          (1<<TWIE)|(1<<TWINT)|              // enable interrupt and reset
          (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO);   // transmit START
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
   g_bAddrByte = nSlaveAddr | (1<<ADDR_READ_BIT);
   g_pbMessage = (PBYTE)pvMessage;
   g_cbMessage = cbBuffer;
   g_bXferOk    = FALSE;
   // start the data transfer
   TWCR = (1<<TWEN)|                         // I2C enabled
          (1<<TWIE)|(1<<TWINT)|              // enable interrupt and reset
          (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO);   // transmit START
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
         TWDR = g_bAddrByte;                          // transmit the address byte
         TWCR = (1<<TWEN)|                            // I2C enabled
                (1<<TWIE)|(1<<TWINT)|                 // enable interrupt and reset
                (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);      // transmit data
         break;
      case STATUS_ARB_LOST:                        // arbitration lost
         TWCR = (1<<TWEN)|                            // I2C enabled
                (1<<TWIE)|(1<<TWINT)|                 // enable interrupt and reset
                (0<<TWEA)|(1<<TWSTA)|(0<<TWSTO);      // transmit re-START
         break;
      case STATUS_MTX_ADR_ACK:                     // address byte transmitted
      case STATUS_MTX_DATA_ACK:                    // data byte transmitted
         if (nMsgIdx < g_cbMessage)
         {
            TWDR = g_pbMessage[nMsgIdx++];            // transmit the current data byte
            TWCR = (1<<TWEN)|                         // I2C enabled
                   (1<<TWIE)|(1<<TWINT)|              // enable interrupt and reset
                   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);   // transmit data
         }
         else
         {
            g_bXferOk = TRUE;
            TWCR = (1<<TWEN)|                         // I2C enabled
                   (0<<TWIE)|(1<<TWINT)|              // disable interrupt and reset
                   (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO);   // transmit STOP
         }
         break;
      case STATUS_MRX_ADR_ACK:                     // address byte transmitted
         g_bAddrByte = TWDR;
         break;
      case STATUS_MRX_DATA_ACK:                    // data byte received
         g_pbMessage[nMsgIdx++] = TWDR;
         if (nMsgIdx < g_cbMessage - 1)
            TWCR = (1<<TWEN)|                         // I2C enabled
                   (1<<TWIE)|(1<<TWINT)|              // enable interrupt and reset
                   (1<<TWEA)|(0<<TWSTA)|(0<<TWSTO);   // transmit ACK
         else
            TWCR = (1<<TWEN)|                         // I2C enabled
                   (1<<TWIE)|(1<<TWINT)|              // enable interrupt and reset
                   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);   // transmit NACK
         break;
      case STATUS_MRX_DATA_NACK:                   // NACK transmitted
         g_pbMessage[nMsgIdx++] = TWDR;
         g_cbMessage = nMsgIdx;
         g_bXferOk = TRUE;
         TWCR = (1<<TWEN)|                            // I2C enabled
                (0<<TWIE)|(1<<TWINT)|                 // disable interrupt and reset
                (0<<TWEA)|(0<<TWSTA)|(1<<TWSTO);      // transmit STOP
         break;
      case STATUS_MRX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_MTX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_MTX_DATA_NACK:                   // error - NACK was received after data
      case STATUS_BUS_ERROR:                       // general error
      default:                                     // unknown error
         g_cbMessage = nMsgIdx;
         TWCR = (1<<TWEN)|                            // I2C enabled
                (0<<TWIE)|(1<<TWINT)|                 // disable interrupt and reset
                (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO);      // abort
         break;
   }
}
