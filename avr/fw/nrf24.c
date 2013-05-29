//===========================================================================
// Module:  nrf24.c
// Purpose: Nordic RF transfer module NRF24L01+
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
#include "nrf24.h"
#include "spimast.h"
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// NRF24 COMMANDS
//===========================================================================
#define COMMAND_READREGISTER        0x00
#define COMMAND_WRITEREGISTER       0x20
#define COMMAND_RXREADPACKET        0x61
#define COMMAND_TXWRITEPACKET       0xA0
#define COMMAND_TXFLUSH             0xE1
#define COMMAND_RXFLUSH             0xE2
#define COMMAND_TXREUSE             0xE3
#define COMMAND_ACTIVATE            0x50
#define COMMAND_TXWRITENOACK        0xB0
#define COMMAND_NOOP                0xFF
//===========================================================================
// NRF24 REGISTER ADDRESSES
//===========================================================================
#define REGISTER_CONFIG             0x00
#define REGISTER_AUTOACK            0x01
#define REGISTER_RXENABLED          0x02
#define REGISTER_ADDRESSWIDTH       0x03
#define REGISTER_AUTORETRY          0x04
#define REGISTER_RFCHANNEL          0x05
#define REGISTER_RFCONFIG           0x06
#define REGISTER_STATUS             0x07
#define REGISTER_TXSTATS            0x08
#define REGISTER_CARRIERDETECT      0x09
#define REGISTER_RXADDRESS0         0x0A
#define REGISTER_RXADDRESS1         0x0B
#define REGISTER_RXADDRESS2         0x0C
#define REGISTER_RXADDRESS3         0x0D
#define REGISTER_RXADDRESS4         0x0E
#define REGISTER_RXADDRESS5         0x0F
#define REGISTER_TXADDRESS          0x10
#define REGISTER_RXLENGTH0          0x11
#define REGISTER_RXLENGTH1          0x12
#define REGISTER_RXLENGTH2          0x13
#define REGISTER_RXLENGTH3          0x14
#define REGISTER_RXLENGTH4          0x15
#define REGISTER_RXLENGTH5          0x16
#define REGISTER_FIFOSTATUS         0x17
#define REGISTER_DYNPAYLOAD         0x1C
#define REGISTER_FEATURE            0x1D
//-------------------[        Module Variables         ]-------------------//
static UI8  g_nSsPin       = PIN_INVALID;          // slave select pin
static UI8  g_nCePin       = PIN_INVALID;          // chip enable pin
static UI8  g_cbAddress    = 5;                    // address width
static BOOL g_bTXRecvAck   = TRUE;                 // enable TX acks?
static BOOL g_fPowerMode   = NRF24_MODE_OFF;       // powered up?
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: ReadRegister >--------------------------------------
// Purpose:    reads an NRF24 register over SPI
// Parameters: nRegister - register address
//             pvData    - return register data via here
//             cbData    - the number of bytes to read
// Returns:    pvData
//---------------------------------------------------------------------------
static PVOID ReadRegister (UI8 nRegister, PVOID pvData, UI8 cbData)
{
   UI8  cbSendRecv = cbData + 1;
   BYTE pbSendRecv[cbSendRecv];
   pbSendRecv[0] = COMMAND_READREGISTER | (nRegister & 0x1F);
   SpiSendRecv(g_nSsPin, pbSendRecv, cbSendRecv, pbSendRecv, cbSendRecv);
   memcpy(pvData, pbSendRecv + 1, cbData);
   return pvData;
}
//-----------< FUNCTION: WriteRegister >-------------------------------------
// Purpose:    writes an NRF24 register over SPI
// Parameters: nRegister - register address
//             pvData    - register data to write
//             cbData    - the number of bytes to write
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister (UI8 nRegister, PVOID pvData, UI8 cbData)
{
   UI8  cbSend = cbData + 1;
   BYTE pbSend[cbSend];
   pbSend[0] = COMMAND_WRITEREGISTER | (nRegister & 0x1F);
   memcpy(pbSend + 1, pvData, cbData);
   SpiSend(g_nSsPin, pbSend, cbSend);
}
//-----------< FUNCTION: ReadRegister8 >-------------------------------------
// Purpose:    reads an 8-bit NRF24 register
// Parameters: nRegister - register address
// Returns:    the register value
//---------------------------------------------------------------------------
static UI8 ReadRegister8 (UI8 nRegister)
{
   BYTE pbSend[1] = { COMMAND_READREGISTER | (nRegister & 0x1F) };
   BYTE pbRecv[2];
   SpiSendRecv(g_nSsPin, pbSend, sizeof(pbSend), pbRecv, sizeof(pbRecv));
   return pbRecv[1];
}
//-----------< FUNCTION: WriteRegister8 >------------------------------------
// Purpose:    writes an 8-bit NRF24 register
// Parameters: nRegister - register address
//             nValue    - value to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister8 (UI8 nRegister, UI8 nValue)
{
   BYTE pbSend[2] = { COMMAND_WRITEREGISTER | (nRegister & 0x1F), nValue };
   SpiSend(g_nSsPin, pbSend, sizeof(pbSend));
}
//-----------< FUNCTION: ReadStatus >----------------------------------------
// Purpose:    reads the NRF24 status register
//             uses the no-op instruction, which returns the status register
//             with a single byte transfer
// Parameters: none
// Returns:    the contents of the status register
//---------------------------------------------------------------------------
static UI8 ReadStatus ()
{
   BYTE pbSendRecv[1] = { COMMAND_NOOP };
   SpiSendRecv(g_nSsPin, pbSendRecv, 1, pbSendRecv, 1);
   return pbSendRecv[0];
}
//-----------< FUNCTION: Nrf24Init >----------------------------------------
// Purpose:    NRF24 interface initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24Init (PNRF24_CONFIG pConfig)
{
   // initialize output pins
   g_nSsPin = pConfig->nSsPin;
   g_nCePin = pConfig->nCePin;
   if (g_nSsPin != PIN_INVALID)
   {
      PinSetOutput(g_nSsPin);
      PinSetHi(g_nSsPin);
   }
   PinSetOutput(g_nCePin);
   PinSetLo(g_nCePin);
   // initialize registers to power on reset values
   Nrf24SetIrqMask(NRF24_IRQ_NONE);
   Nrf24SetCrc(NRF24_CRC_8BIT);
   Nrf24SetAutoAck(NRF24_PIPE_ALL);
   Nrf24SetRXEnabled(BitMask(NRF24_PIPE0) | BitMask(NRF24_PIPE1));
   Nrf24SetAddressSize(5);
   Nrf24SetRetryDelay(250);
   Nrf24SetRetryCount(15);
   Nrf24SetRFChannel(2);
   Nrf24SetRFDataRate(NRF24_RATE_2MBPS);
   Nrf24SetRFPower(NRF24_POWER_MINUS0DBM);
   Nrf24SetLnaGain(TRUE);
   // initialize TX/RX default addresses (rx: nrf00-nrf05 tx: nrf00)
   CHAR szAddress[5 + 1] = "nrf00";
   Nrf24SetTXAddress(szAddress);
   for (UI8 i = 0; i < NRF24_PIPE_COUNT; i++)
   {
      szAddress[4] = '0' + i;
      Nrf24SetRXAddress(i, szAddress);
      Nrf24SetPayloadLength(i, 0);
   }
   Nrf24SetDynPayload(NRF24_PIPE_NONE);
   // flush dynamic registers
   Nrf24ClearIrq(NRF24_IRQ_ALL);
   Nrf24FlushSend();
   Nrf24FlushRecv();
}
//-----------< FUNCTION: Nrf24GetIrqMask >-----------------------------------
// Purpose:    gets the currently masked IRQs from the CONFIG register
// Parameters: none
// Returns:    IRQ mask value
//---------------------------------------------------------------------------
UI8 Nrf24GetIrqMask ()
{
   return ReadRegister8(REGISTER_CONFIG) & NRF24_IRQ_ALL;
}
//-----------< FUNCTION: Nrf24SetIrqMask >-----------------------------------
// Purpose:    sets the currently masked IRQs in the CONFIG register
// Parameters: fMask - IRQ mask value
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetIrqMask (UI8 fMask)
{
   WriteRegister8(
      REGISTER_CONFIG,
      (ReadRegister8(REGISTER_CONFIG) & ~NRF24_IRQ_ALL) | 
      (fMask & NRF24_IRQ_ALL)
   );
}
//-----------< FUNCTION: Nrf24GetCrc >---------------------------------------
// Purpose:    gets the CRC mode from the CONFIG register
// Parameters: none
// Returns:    CRC value (none, 8-bit, or 16-bit)
//---------------------------------------------------------------------------
UI8 Nrf24GetCrc ()
{
   return ReadRegister8(REGISTER_CONFIG) & 0x0C;
}
//-----------< FUNCTION: Nrf24SetCrc >---------------------------------------
// Purpose:    sets the CRC mode in the CONFIG register
// Parameters: fCrc - CRC value (none, 8-bit, or 16-bit)
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetCrc (UI8 fCrc)
{
   WriteRegister8(
      REGISTER_CONFIG,
      (ReadRegister8(REGISTER_CONFIG) & ~0x0C) | 
      (fCrc & 0x0C)
   );
}
//-----------< FUNCTION: Nrf24GetAutoack >-----------------------------------
// Purpose:    gets the EN_AA bitmask register
// Parameters: none
// Returns:    auto-ack register value (1 bit per pipe)
//---------------------------------------------------------------------------
UI8 Nrf24GetAutoAck ()
{
   return ReadRegister8(REGISTER_AUTOACK);
}
//-----------< FUNCTION: Nrf24SetAutoAck >-----------------------------------
// Purpose:    sets the EN_AA bitmask register
// Parameters: fAutoAck - auto-ack register value (1 bit per pipe)
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetAutoAck (UI8 fAutoAck)
{
   WriteRegister8(REGISTER_AUTOACK, fAutoAck & 0x3F);
}
//-----------< FUNCTION: Nrf24GetRXEnabled >---------------------------------
// Purpose:    gets the EN_RXADDR register
// Parameters: none
// Returns:    RX enabled register value (1 bit per pipe)
//---------------------------------------------------------------------------
UI8 Nrf24GetRXEnabled ()
{
   return ReadRegister8(REGISTER_RXENABLED);
}
//-----------< FUNCTION: Nrf24SetRXEnabled >---------------------------------
// Purpose:    sets the EN_RXADDR register
// Parameters: fRXEnabled - RX enabled register value (1 bit per pipe)
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRXEnabled (UI8 fRXEnabled)
{
   WriteRegister8(REGISTER_RXENABLED, fRXEnabled & 0x3F);
}
//-----------< FUNCTION: Nrf24GetAddressSize >-------------------------------
// Purpose:    gets the SETUP_AW register
// Parameters: none
// Returns:    TX/RX address length, in bytes
//---------------------------------------------------------------------------
UI8 Nrf24GetAddressSize ()
{
   return ReadRegister8(REGISTER_ADDRESSWIDTH) + 2;
}
//-----------< FUNCTION: Nrf24SetAddressSize >-------------------------------
// Purpose:    sets the SETUP_AW register
// Parameters: cbAddress - TX/RX address length, in bytes
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetAddressSize (UI8 cbAddress)
{
   if (cbAddress >= 3 && cbAddress <= 5)
   {
      WriteRegister8(REGISTER_ADDRESSWIDTH, cbAddress - 2);
      g_cbAddress = cbAddress;
   }
}
//-----------< FUNCTION: Nrf24GetRetryDelay >--------------------------------
// Purpose:    gets the retry delay from the SETUP_RETR register
// Parameters: none
// Returns:    retry delay, in microseconds
//---------------------------------------------------------------------------
UI8 Nrf24GetRetryDelay ()
{
   return ((ReadRegister8(REGISTER_AUTORETRY) >> 4) + 1) * 250;
}
//-----------< FUNCTION: Nrf24SetRetryDelay >--------------------------------
// Purpose:    sets the retry delay in the SETUP_RETR register
// Parameters: nDelay - retry delay, in microseconds
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRetryDelay (UI8 nDelay)
{
   WriteRegister8(
      REGISTER_AUTORETRY,
      (ReadRegister8(REGISTER_AUTORETRY) & ~0xF0) | 
      ((nDelay / 250 - 1) << 4)
   );
}
//-----------< FUNCTION: Nrf24GetRetryCount >--------------------------------
// Purpose:    gets the retry count from the SETUP_RETR register
// Parameters: none
// Returns:    retry count
//---------------------------------------------------------------------------
UI8 Nrf24GetRetryCount ()
{
   return ReadRegister8(REGISTER_AUTORETRY) & 0x0F;
}
//-----------< FUNCTION: Nrf24SetRetryCount >--------------------------------
// Purpose:    sets the retry count in the SETUP_RETR register
// Parameters: nCount - retransmit count
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRetryCount (UI8 nCount)
{
   WriteRegister8(
      REGISTER_AUTORETRY,
      (ReadRegister8(REGISTER_AUTORETRY) & ~0x0F) | 
      (nCount & 0x0F)
   );
}
//-----------< FUNCTION: Nrf24GetRFChannel >---------------------------------
// Purpose:    gets the RF_CH register
// Parameters: none
// Returns:    current RF channel, in mHz over 2.4gHz
//---------------------------------------------------------------------------
UI8 Nrf24GetRFChannel ()
{
   return ReadRegister8(REGISTER_RFCHANNEL);
}
//-----------< FUNCTION: Nrf24SetRFChannel >---------------------------------
// Purpose:    sets the RF_CH register
// Parameters: nChannel - current RF channel, in mHz over 2.4gHz
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRFChannel (UI8 nChannel)
{
   // TODO: trace on validation failures
   if (nChannel < 84)
      WriteRegister8(REGISTER_RFCHANNEL, nChannel);
}
//-----------< FUNCTION: Nrf24GetRFDataRate >--------------------------------
// Purpose:    gets the RF data rate from the RF_SETUP register
// Parameters: none
// Returns:    an NRF24_RATE_* value representing the transfer speed
//---------------------------------------------------------------------------
UI8 Nrf24GetRFDataRate ()
{
   return ReadRegister8(REGISTER_RFCONFIG) & 0x8;
}
//-----------< FUNCTION: Nrf24SetRFDataRate >--------------------------------
// Purpose:    sets the RF data rate in the RF_SETUP register
// Parameters: fDataRate - an NRF24_RATE_* representing the transfer speed
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRFDataRate (UI8 fDataRate)
{
   WriteRegister8(
      REGISTER_RFCONFIG,
      (ReadRegister8(REGISTER_RFCONFIG) & ~0x8) | fDataRate
   );
}
//-----------< FUNCTION: Nrf24GetRFPower >-----------------------------------
// Purpose:    gets the RF power level from the RF_SETUP register
// Parameters: none
// Returns:    an NRF24_POWER_* value representing the power level
//---------------------------------------------------------------------------
UI8 Nrf24GetRFPower ()
{
   return (ReadRegister8(REGISTER_RFCONFIG) >> 2) & 0x3;
}
//-----------< FUNCTION: Nrf24SetRFPower >-----------------------------------
// Purpose:    sets the RF power level in the RF_SETUP register
// Parameters: fPower - an NRF24_POWER_* representing the power level
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRFPower (UI8 fPower)
{
   WriteRegister8(
      REGISTER_RFCONFIG,
      (ReadRegister8(REGISTER_RFCONFIG) & ~0x6) |
      ((fPower & 0x3) << 1)
   );
}
//-----------< FUNCTION: Nrf24GetLnaGain >-----------------------------------
// Purpose:    gets the low noise amplifer bit from the RF_SETUP register
// Parameters: none
// Returns:    the LNA gain bit value
//---------------------------------------------------------------------------
BOOL Nrf24GetLnaGain ()
{
   return ReadRegister8(REGISTER_RFCONFIG) & 0x1;
}
//-----------< FUNCTION: Nrf24SetLnaGain >-----------------------------------
// Purpose:    sets the low noise amplifer bit in the RF_SETUP register
// Parameters: bLnaGain - LNA gain enable value
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetLnaGain (BOOL bLnaGain)
{
   WriteRegister8(
      REGISTER_RFCONFIG,
      (ReadRegister8(REGISTER_RFCONFIG) & ~0x1) | 
      ((bLnaGain & 0x1) << 0)
   );
}
//-----------< FUNCTION: Nrf24GetIrq >---------------------------------------
// Purpose:    gets the interrupts currently set in the STATUS register
// Parameters: none
// Returns:    a bitmask containing the interrupts that are set
//---------------------------------------------------------------------------
UI8 Nrf24GetIrq ()
{
   return ReadStatus() & NRF24_IRQ_ALL;
}
//-----------< FUNCTION: Nrf24GetRXPipe >------------------------------------
// Purpose:    gets the number of the RX pipe that contains data,
//             from the STATUS register
// Parameters: none
// Returns:    the RX pipe containing data, if any
//             NRF24_PIPE_UNKNOWN otherwise
//---------------------------------------------------------------------------
UI8 Nrf24GetRXPipe ()
{
   UI8 fPipe = (ReadStatus() >> 1) & 0x7;
   return (fPipe != 0x7) ? fPipe : NRF24_PIPE_UNKNOWN;
}
//-----------< FUNCTION: Nrf24GetLostPackets >-------------------------------
// Purpose:    gets the lost packet count from the OBSERVE_TX register
// Parameters: none
// Returns:    the number of lost packets since the last RF_CH change
//---------------------------------------------------------------------------
UI8 Nrf24GetLostPackets ()
{
   return ReadRegister8(REGISTER_TXSTATS) >> 4;
}
//-----------< FUNCTION: Nrf24GetRetransmits >-------------------------------
// Purpose:    gets the retransmit count from the OBSERVE_TX register
// Parameters: none
// Returns:    the number of retransmits since the last packet was started
//---------------------------------------------------------------------------
UI8 Nrf24GetRetransmits ()
{
   return ReadRegister8(REGISTER_TXSTATS) & 0x0F;
}
//-----------< FUNCTION: Nrf24GetCarrier >-----------------------------------
// Purpose:    gets the carrier detect flag from the CD register
// Parameters: none
// Returns:    true if a transceiver was detected
//             false otherwise
//---------------------------------------------------------------------------
BOOL Nrf24GetCarrier ()
{
   return ReadRegister8(REGISTER_CARRIERDETECT) ? TRUE : FALSE;
}
//-----------< FUNCTION: Nrf24GetRXAddress >---------------------------------
// Purpose:    gets the receive address for an RX pipe from the
//             RX_ADDR_P* registers
// Parameters: nPipe      - the number of the pipe to query
//             pszAddress - return the pipe address via here
// Returns:    pszAddress
//---------------------------------------------------------------------------
PSTR Nrf24GetRXAddress (UI8 nPipe, PSTR pszAddress)
{
   if (nPipe < NRF24_PIPE_COUNT && pszAddress != NULL)
   {
      BYTE pbAddress[g_cbAddress];
      if (nPipe == 0)
      {
         // pipe 0 has its own 5-byte address register
         ReadRegister(REGISTER_RXADDRESS0, pbAddress, g_cbAddress);
         for (UI8 i = 0; i < g_cbAddress; i++)
            pszAddress[i] = pbAddress[g_cbAddress - i - 1];
      }
      else
      {
         // pipes 1-5 share the first 4 bytes of an address register
         ReadRegister(REGISTER_RXADDRESS1, pbAddress, g_cbAddress);
         for (UI8 i = 0; i < g_cbAddress - 1; i++)
            pszAddress[i] = pbAddress[g_cbAddress - i - 1];
         if (nPipe > 1)
            pszAddress[g_cbAddress - 1] = ReadRegister8(REGISTER_RXADDRESS0 + nPipe);
      }
      pszAddress[g_cbAddress] = '\0';
      return pszAddress;
   }
   return NULL;
}
//-----------< FUNCTION: Nrf24SetRXAddress >---------------------------------
// Purpose:    sets the receive address for an RX pipe in the
//             RX_ADDR_P* registers
// Parameters: nPipe      - the number of the pipe to assign
//             pszAddress - the pipe address to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetRXAddress (UI8 nPipe, PCSTR pszAddress)
{
   if (nPipe < NRF24_PIPE_COUNT && pszAddress != NULL && strlen(pszAddress) == g_cbAddress)
   {
      if (nPipe <= 1)
      {
         // write a 5-byte address for pipes 0 and 1
         BYTE pbAddress[g_cbAddress];
         for (UI8 i = 0; i < g_cbAddress; i++)
            pbAddress[i] = pszAddress[g_cbAddress - i - 1];
         WriteRegister(REGISTER_RXADDRESS0 + nPipe, pbAddress, g_cbAddress);
      }
      else
      {
         // only write the LSB for pipes 2-5
         WriteRegister8(REGISTER_RXADDRESS0 + nPipe, pszAddress[g_cbAddress - 1]);
      }
   }
}
//-----------< FUNCTION: Nrf24GetTXAddress >---------------------------------
// Purpose:    gets the transmit address from the TX_ADDR register
// Parameters: pszAddress - return the address via here
// Returns:    pszAddress
//---------------------------------------------------------------------------
PSTR Nrf24GetTXAddress (PSTR pszAddress)
{
   if (pszAddress != NULL)
   {
      BYTE pbAddress[g_cbAddress];
      ReadRegister(REGISTER_TXADDRESS, pbAddress, g_cbAddress);
      for (UI8 i = 0; i < g_cbAddress; i++)
         pszAddress[i] = pbAddress[g_cbAddress - i - 1];
      pszAddress[g_cbAddress] = '\0';
      return pszAddress;
   }
   return NULL;
}
//-----------< FUNCTION: Nrf24SetTXAddress >---------------------------------
// Purpose:    sets the transmit address in the TX_ADDR register
// Parameters: pszAddress - the transmit address to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetTXAddress (PCSTR pszAddress)
{
   if (pszAddress != NULL && strlen(pszAddress) == g_cbAddress)
   {
      BYTE pbAddress[g_cbAddress];
      for (UI8 i = 0; i < g_cbAddress; i++)
         pbAddress[i] = pszAddress[g_cbAddress - i - 1];
      WriteRegister(REGISTER_TXADDRESS, pbAddress, g_cbAddress);
   }
}
//-----------< FUNCTION: Nrf24GetPayloadLength >-----------------------------
// Purpose:    gets the payload length for an RX pipe from the
//             RX_PW_P* registers
// Parameters: nPipe - the number of the pipe to query
// Returns:    the expected or actual payload length for the pipe
//---------------------------------------------------------------------------
UI8 Nrf24GetPayloadLength (UI8 nPipe)
{
   if (nPipe < NRF24_PIPE_COUNT)
      return ReadRegister8(REGISTER_RXLENGTH0 + nPipe);
   return 0;
}
//-----------< FUNCTION: Nrf24SetPayloadLength >-----------------------------
// Purpose:    sets the expected payload length for an RX pipe in the
//             RX_PW_P* registers
// Parameters: nPipe     - the number of the pipe to assign
//             cbPayload - expected payload length, in bytes, for the pipe
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetPayloadLength (UI8 nPipe, UI8 cbPayload)
{
   if (nPipe < NRF24_PIPE_COUNT && cbPayload <= NRF24_PACKET_MAX)
      WriteRegister8(REGISTER_RXLENGTH0 + nPipe, cbPayload);
}
//-----------< FUNCTION: Nrf24GetFifoStatus >--------------------------------
// Purpose:    gets the FIFO_STATUS register value
//             RX_ADDR_P* registers
// Parameters: none
// Returns:    the status of the TX/RX queues
//---------------------------------------------------------------------------
UI8 Nrf24GetFifoStatus ()
{
   return ReadRegister8(REGISTER_FIFOSTATUS);
}
//-----------< FUNCTION: Nrf24GetDynPayload >--------------------------------
// Purpose:    gets the DYNPD register
// Parameters: none
// Returns:    dynamic payload enabled register value (1 bit per pipe)
//---------------------------------------------------------------------------
UI8 Nrf24GetDynPayload ()
{
   return ReadRegister8(REGISTER_DYNPAYLOAD);
}
//-----------< FUNCTION: Nrf24SetDynPayload >--------------------------------
// Purpose:    gets the DYNPD register
// Parameters: fDynPayload - dynamic payload enabled value (1 bit per pipe)
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetDynPayload (UI8 fDynPayload)
{
   WriteRegister8(REGISTER_DYNPAYLOAD, fDynPayload & 0x3F);
}
//-----------< FUNCTION: Nrf24GetFeatures >----------------------------------
// Purpose:    gets the contents of the FEATURE register
// Parameters: none
// Returns:    enabled features bit mask
//---------------------------------------------------------------------------
UI8 Nrf24GetFeatures ()
{
   return ReadRegister8(REGISTER_FEATURE);
}
//-----------< FUNCTION: Nrf24SetFeatures >----------------------------------
// Purpose:    sets the contents of the FEATURE register
// Parameters: fFeatures - enabled features bit mask
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24SetFeatures (UI8 fFeatures)
{
   WriteRegister8(REGISTER_FEATURE, fFeatures & 0x7);
   g_bTXRecvAck = !(fFeatures & NRF24_FEATURE_DISABLEACK);
}
//-----------< FUNCTION: Nrf24ClearIrq >-------------------------------------
// Purpose:    clears interrupts currently set
// Parameters: fIrq - bitmask of interrupts to clear
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24ClearIrq (UI8 fIrq)
{
   WriteRegister8(REGISTER_STATUS, fIrq & NRF24_IRQ_ALL);
}
//-----------< FUNCTION: Nrf24FlushSend >------------------------------------
// Purpose:    empties the transceiver's TX FIFO
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24FlushSend ()
{
   SpiSend(g_nSsPin, (BYTE[]){ COMMAND_TXFLUSH }, 1);
}
//-----------< FUNCTION: Nrf24FlushRecv >------------------------------------
// Purpose:    empties the transceiver's RX FIFO
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24FlushRecv ()
{
   SpiSend(g_nSsPin, (BYTE[]){ COMMAND_RXFLUSH }, 1);
}
//-----------< FUNCTION: Nrf24PowerOn >--------------------------------------
// Purpose:    starts up the transceiver
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24PowerOn (UI8 fMode)
{
   if (fMode == NRF24_MODE_SEND || fMode == NRF24_MODE_RECV)
   {
      WriteRegister8(
         REGISTER_CONFIG,
         (ReadRegister8(REGISTER_CONFIG) & ~0x3) | (0x2) | (fMode & 0x1)
      );
      g_fPowerMode = fMode;
   }
}
//-----------< FUNCTION: Nrf24PowerOff >-------------------------------------
// Purpose:    shuts down the transceiver
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24PowerOff ()
{
   g_fPowerMode = NRF24_MODE_OFF;
   WriteRegister8(
      REGISTER_CONFIG,
      ReadRegister8(REGISTER_CONFIG) & ~0x3
   );
}
//-----------< FUNCTION: Nrf24Send >-----------------------------------------
// Purpose:    transmits a packet
// Parameters: pvPacket - packet to transfer
//             cbPacket - number of bytes to transfer
// Returns:    none
//---------------------------------------------------------------------------
VOID Nrf24Send (PCVOID pvPacket, BSIZE cbPacket)
{
   // TODO: trace failure
   if (g_fPowerMode == NRF24_MODE_SEND)
   {
      // set CE high to move the transciever out of standby
      PinSetHi(g_nCePin);
      // clock in the command and data buffer
      UI8  cbSend = cbPacket + 1;
      BYTE pbSend[cbSend];
      pbSend[0] = g_bTXRecvAck ? COMMAND_TXWRITEPACKET : COMMAND_TXWRITENOACK;
      memcpy(pbSend + 1, pvPacket, MIN(cbPacket, NRF24_PACKET_MAX));
      SpiSend(g_nSsPin, pbSend, cbSend);
      SpiWait();
      // set CE low to return to standby after the transfer
      PinSetLo(g_nCePin);
   }
}
