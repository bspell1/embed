//===========================================================================
// Module:  nrf24.h
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
#ifndef __NRF24_H
#define __NRF24_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// NRF24 STRUCTURES
//===========================================================================
// module configuration
typedef struct tagNrf24Config
{
   UI8   nSsPin;                          // SPI slave select pin
   UI8   nCePin;                          // NRF24 chip enable, activates RX/TX
} NRF24_CONFIG, *PNRF24_CONFIG;
//===========================================================================
// NRF24 CONFIGURATION VALUES
//===========================================================================
// transceiver mode
#define NRF24_MODE_OFF           0x00     // power
#define NRF24_MODE_SEND          0x02     // power up in transmit mode
#define NRF24_MODE_RECV          0x03     // power up in receive mode
// pipe configuration
#define NRF24_PIPE_COUNT         6        // number of receive pipes
#define NRF24_PIPE_UNKNOWN       0xFF     // for initialization
#define NRF24_PIPE_NONE          0x00     // for pipe flags
#define NRF24_PIPE_ALL           0x3F     // for pipe flags
#define NRF24_PIPE0              0x00     // receive pipe #0
#define NRF24_PIPE1              0x01     // receive pipe #1
#define NRF24_PIPE2              0x02     // receive pipe #2
#define NRF24_PIPE3              0x03     // receive pipe #3
#define NRF24_PIPE4              0x04     // receive pipe #4
#define NRF24_PIPE5              0x05     // receive pipe #5
// buffer parameters
#define NRF24_PACKET_MAX         32       // maximum send/receive length
// interrupts
#define NRF24_IRQ_NONE           0x00     // for initialization
#define NRF24_IRQ_RX_DR          0x40     // RX data ready interrupt
#define NRF24_IRQ_TX_DS          0x20     // TX data ready interrupt
#define NRF24_IRQ_MAX_RT         0x10     // maximum retry exceeded interrupt
#define NRF24_IRQ_ALL            0x70     // all interrupts
// CRC flags
#define NRF24_CRC_NONE           0x00     // no CRC
#define NRF24_CRC_8BIT           0x08     // 1 byte CRC
#define NRF24_CRC_16BIT          0x0C     // 2 byte CRC
// RF data rates
#define NRF24_RATE_1MBPS         0x00     // 1 mbps data rate
#define NRF24_RATE_2MBPS         0x08     // 2 mbps data rate
// RF power modes
#define NRF24_POWER_MINUS0DBM    0x03     // -0dbm
#define NRF24_POWER_MINUS6DBM    0x02     // -6dbm
#define NRF24_POWER_MINUS12DBM   0x01     // -12dbm
#define NRF24_POWER_MINUS18DBM   0x00     // -18dbm
// FIFO status values
#define NRF24_FIFO_TX_FULL       0x20     // transmit FIFO full flag
#define NRF24_FIFO_TX_EMPTY      0x10     // transmit FIFO empty flag
#define NRF24_FIFO_RX_FULL       0x02     // receive FIFO full flag
#define NRF24_FIFO_RX_EMPTY      0x01     // receive FIFO empty flag
// feature register values
#define NRF24_FEATURE_DYNPAYLOAD 0x04     // enable dynamic payloads
#define NRF24_FEATURE_ACKPAYLOAD 0x02     // enable acks with payloads
#define NRF24_FEATURE_DISABLEACK 0x01     // disable acks on TX side
//===========================================================================
// MODULE INITIALIZATION
//===========================================================================
// API initialiization
VOID     Nrf24Init               (PNRF24_CONFIG pConfig);
//===========================================================================
// REGISTER ACCESS
//===========================================================================
// CONFIG register
UI8      Nrf24GetIrqMask         ();
VOID     Nrf24SetIrqMask         (UI8 fMask);
UI8      Nrf24GetCrc             ();
VOID     Nrf24SetCrc             (UI8 fCrc);
// EN_AA register 
UI8      Nrf24GetAutoAck         ();
VOID     Nrf24SetAutoAck         (UI8 fAutoAck);
// EN_RXADDR register   
UI8      Nrf24GetRXEnabled       ();
VOID     Nrf24SetRXEnabled       (UI8 fRXEnabled);
// SETUP_AW register 
UI8      Nrf24GetAddressSize     ();
VOID     Nrf24SetAddressSize     (UI8 cbAddress);
// SETUP_RETR register  
UI8      Nrf24GetRetryDelay      ();
VOID     Nrf24SetRetryDelay      (UI8 nDelay);
UI8      Nrf24GetRetryCount      ();
VOID     Nrf24SetRetryCount      (UI8 nCount);
// RF_CH register 
UI8      Nrf24GetRFChannel       ();
VOID     Nrf24SetRFChannel       (UI8 nChannel);
// RF_SETUP register 
UI8      Nrf24GetRFDataRate      ();
VOID     Nrf24SetRFDataRate      (UI8 fDataRate);
UI8      Nrf24GetRFPower         ();
VOID     Nrf24SetRFPower         (UI8 fPower);
BOOL     Nrf24GetLnaGain         ();
VOID     Nrf24SetLnaGain         (BOOL bLnaGain);
// STATUS register   
UI8      Nrf24GetIrq             ();
UI8      Nrf24GetRXPipe          ();
// OBSERVE_TX register  
UI8      Nrf24GetLostPackets     ();
UI8      Nrf24GetRetransmits     ();
// CD register 
BOOL     Nrf24GetCarrier         ();
// RX_ADDR registers 
PSTR     Nrf24GetRXAddress       (UI8 nPipe, PSTR pszAddress);
VOID     Nrf24SetRXAddress       (UI8 nPipe, PCSTR pszAddress);
// TX_ADDR register  
PSTR     Nrf24GetTXAddress       (PSTR pszAddress);
VOID     Nrf24SetTXAddress       (PCSTR pszAddress);
// RX_PW registers
UI8      Nrf24GetPayloadLength   (UI8 nPipe);
VOID     Nrf24SetPayloadLength   (UI8 nPipe, UI8 cbPayload);
// FIFO_STATUS register
UI8      Nrf24GetFifoStatus      ();
// DYNPD register
UI8      Nrf24GetDynPayload      ();
VOID     Nrf24SetDynPayload      (UI8 fDynPayload);
// FEATURES register
UI8      Nrf24GetFeatures        ();
VOID     Nrf24SetFeatures        (UI8 fFeatures);
// pipe register helpers
inline BOOL Nrf24GetPipeAutoAck (UI8 nPipe)
   { return (Nrf24GetAutoAck() & BitMask(nPipe)) != 0; }
inline VOID Nrf24SetPipeAutoAck (UI8 nPipe, BOOL value)
   { Nrf24SetAutoAck((Nrf24GetAutoAck() & BitUnmask(nPipe)) | (value << nPipe)); }
inline BOOL Nrf24GetPipeRXEnabled (UI8 nPipe)
   { return (Nrf24GetRXEnabled() & BitMask(nPipe)) != 0; }
inline VOID Nrf24SetPipeRXEnabled (UI8 nPipe, BOOL value)
   { Nrf24SetRXEnabled((Nrf24GetRXEnabled() & BitUnmask(nPipe)) | (value << nPipe)); }
inline BOOL Nrf24GetPipeDynPayload (UI8 nPipe)
   { return (Nrf24GetDynPayload() & BitMask(nPipe)) != 0; }
inline VOID Nrf24SetPipeDynPayload (UI8 nPipe, BOOL value)
   { Nrf24SetDynPayload((Nrf24GetDynPayload() & BitUnmask(nPipe)) | (value << nPipe)); }
// feature register helpers
inline BOOL Nrf24DynPayloadEnabled ()
   { return Nrf24GetFeatures() & NRF24_FEATURE_DYNPAYLOAD ? TRUE : FALSE; }
inline VOID Nrf24EnableDynPayload ()
   { Nrf24SetFeatures(Nrf24GetFeatures() | NRF24_FEATURE_DYNPAYLOAD); }
inline VOID Nrf24DisableDynPayload ()
   { Nrf24SetFeatures(Nrf24GetFeatures() & ~NRF24_FEATURE_DYNPAYLOAD); }
inline BOOL Nrf24AckPayloadEnabled ()
   { return Nrf24GetFeatures() & NRF24_FEATURE_ACKPAYLOAD ? TRUE : FALSE; }
inline VOID Nrf24EnableAckPayload ()
   { Nrf24SetFeatures(Nrf24GetFeatures() | NRF24_FEATURE_ACKPAYLOAD); }
inline VOID Nrf24AckPayloadDisabled ()
   { Nrf24SetFeatures(Nrf24GetFeatures() & ~NRF24_FEATURE_ACKPAYLOAD); }
inline BOOL Nrf24AckDisabled ()
   { return Nrf24GetFeatures() & NRF24_FEATURE_DISABLEACK ? TRUE : FALSE; }
inline VOID Nrf24DisableAck ()
   { Nrf24SetFeatures(Nrf24GetFeatures() | NRF24_FEATURE_DISABLEACK); }
inline VOID Nrf24EnableAck ()
   { Nrf24SetFeatures(Nrf24GetFeatures() & ~NRF24_FEATURE_DISABLEACK); }
//===========================================================================
// OPERATIONS
//===========================================================================
VOID     Nrf24ClearIrq           (UI8 fIrq);
VOID     Nrf24FlushSend          ();
VOID     Nrf24FlushRecv          ();
VOID     Nrf24PowerOn            (UI8 fMode);
VOID     Nrf24PowerOff           ();
VOID     Nrf24Send               (PCVOID pvPacket, BSIZE cbPacket);
// busy polling helpers
inline BOOL Nrf24IsSendBusy ()
   { return (Nrf24GetFifoStatus() & NRF24_FIFO_TX_FULL) ? TRUE : FALSE; }
inline VOID Nrf24SendWait ()
   { while (Nrf24IsSendBusy()); }
inline BOOL Nrf24IsRecvBusy ()
   { return (Nrf24GetFifoStatus() & NRF24_FIFO_RX_FULL) ? TRUE : FALSE; }
inline VOID Nrf24RecvWait ()
   { while (Nrf24IsRecvBusy()); }
#endif // __NRF24_H  
