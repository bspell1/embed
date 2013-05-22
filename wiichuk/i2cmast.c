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
   (0<<TWIE)|(0<<TWINT)| \
   (0<<TWEA)|(0<<TWSTA)|(0<<TWSTO)
#define CONTROL_XFER_DATA \
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
static BYTE  volatile  g_bAddr     = 0;        // slave address/read bit
static PBYTE volatile  g_pbSend    = NULL;     // send buffer
static BSIZE volatile  g_cbSend    = 0;        // send buffer length
static PBYTE volatile  g_pbRecv    = NULL;     // receive buffer
static BSIZE volatile  g_cbRecv    = 0;        // receive buffer size
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: I2cInit >-------------------------------------------
// Purpose:    I2C interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cInit ()
{
   // initialize I2C registers
   TWSR = 0;                                 // prescale the I2C clock at 0, clear status
   TWBR = (F_CPU / I2C_FREQUENCY - 16) / 2;  // set to 400kHz bit rate
   TWDR = 0xFF;                              // initialize data to SDA clear
   TWCR = CONTROL_INIT;                      // initialize control register
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
   return RegGet(TWCR, TWIE);
}
//-----------< FUNCTION: I2cWait >-------------------------------------------
// Purpose:    waits for the I2C bus to become available
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cWait ()
{
   while (I2cIsBusy())
      ;
}
//-----------< FUNCTION: I2cSend >-------------------------------------------
// Purpose:    sends a message on the I2C interface
// Parameters: nSlaveAddr - address of the slave to send to
//             pvSend     - send message buffer
//             cbSend     - number of bytes to send
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cSend (BYTE nSlaveAddr, PCVOID pvSend, BSIZE cbSend)
{
   I2cWait();
   // set up I2C state
   g_bAddr  = (nSlaveAddr << 1) & BitUnmask(ADDR_READ_BIT);
   g_pbSend = (PBYTE)pvSend;
   g_cbSend = cbSend;
   g_pbRecv = NULL;
   g_cbRecv = 0;
   // start the data transfer
   TWCR = CONTROL_START;
}
//-----------< FUNCTION: I2cRecv >-------------------------------------------
// Purpose:    receives a message on the I2C interface
// Parameters: nSlaveAddr - address of the slave to receive from
//             pvRecv     - receive message buffer
//             cbRecv     - maximum number of bytes to receive
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cRecv (UI8 nSlaveAddr, PVOID pvRecv, BSIZE cbRecv)
{
   I2cWait();
   // set up I2C state
   g_bAddr  = (nSlaveAddr << 1) | BitMask(ADDR_READ_BIT);
   g_pbSend = NULL;
   g_cbSend = 0;
   g_pbRecv = (PBYTE)pvRecv;
   g_cbRecv = cbRecv;
   // start the data transfer
   TWCR = CONTROL_START;
}
//-----------< FUNCTION: I2cSendRecv >---------------------------------------
// Purpose:    executes a combined send/receive transaction on the I2C bus
// Parameters: nSlaveAddr - address of the slave to receive from
//             pvSend     - send message buffer
//             cbSend     - number of bytes to send
//             pvRecv     - receive message buffer
//             cbRecv     - maximum number of bytes to receive
// Returns:    none
//---------------------------------------------------------------------------
VOID I2cSendRecv (
   UI8    nSlaveAddr, 
   PCVOID pvSend, 
   BSIZE  cbSend,
   PVOID  pvRecv, 
   BSIZE  cbRecv)
{
   I2cWait();
   // set up I2C state
   g_bAddr  = (nSlaveAddr << 1) & BitUnmask(ADDR_READ_BIT);
   g_pbSend = (PBYTE)pvSend;
   g_cbSend = cbSend;
   g_pbRecv = (PBYTE)pvRecv;
   g_cbRecv = cbRecv;
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
   static volatile UI8 nMsgIdx = 0;
   switch (TWSR)
   {
      case STATUS_START:                           // START transmitted
      case STATUS_REP_START:                       // re-START transmitted
         nMsgIdx = 0;
         TWDR = g_bAddr;
         TWCR = CONTROL_XFER_DATA;
         break;
      case STATUS_ARB_LOST:                        // arbitration lost, restart
         // combination send/receive transaction,
         // so clear the read bit to restart
         if (g_pbSend != NULL)
            g_bAddr &= BitUnmask(ADDR_READ_BIT);
         TWCR = CONTROL_START;
         break;
      case STATUS_MTX_ADR_ACK:                     // address byte transmitted
      case STATUS_MTX_DATA_ACK:                    // data byte transmitted
         if (nMsgIdx < g_cbSend)
         {
            // keep on sending
            TWDR = g_pbSend[nMsgIdx++];
            TWCR = CONTROL_XFER_DATA;
         }
         else if (g_pbRecv != NULL)
         {
            // send complete, but combination
            // transaction, so restart in read mode
            g_bAddr |= BitMask(ADDR_READ_BIT);
            TWCR = CONTROL_START;
         }
         else
         {
            // send complete, so stop
            TWDR = 0xFF;
            TWCR = CONTROL_STOP;
         }
         break;
      case STATUS_MRX_DATA_ACK:                    // data byte received, ACK transmitted
         g_pbRecv[nMsgIdx++] = TWDR;
      case STATUS_MRX_ADR_ACK:                     // address byte transmitted
         if (nMsgIdx < g_cbRecv - 1)
            TWCR = CONTROL_SEND_ACK;
         else
            TWCR = CONTROL_SEND_NACK;
         break;
      case STATUS_MRX_DATA_NACK:                   // data byte received, NACK transmitted
         g_pbRecv[nMsgIdx++] = TWDR;
         g_cbRecv = nMsgIdx;
         TWDR = 0xFF;
         TWCR = CONTROL_STOP;
         break;
      case STATUS_MTX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_MTX_DATA_NACK:                   // error - NACK was received after data
      case STATUS_MRX_ADR_NACK:                    // error - NACK was received after address
      case STATUS_BUS_ERROR:                       // general error
      default:                                     // unknown error
         TWDR = 0xFF;
         TWCR = CONTROL_ABORT;
         break;
   }
}
