//===========================================================================
// Module:  uart.h
// Purpose: AVR UART driver
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
#ifndef __UART_H
#define __UART_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// UART CONFIGURATION
// . UART_SEND:               enable UART send
// . UART_RECV:               enable UART receive
// . UART_BAUD:               sets the base UART baud rate
// . UART_BAUD_2X:            sets the UART 2x baud rate multiplier
// . UART_SEND_BUFFER_SIZE    sets the UART send buffer size, in bytes
// . UART_RECV_BUFFER_SIZE    sets the UART receive buffer size, in bytes
//===========================================================================
#ifndef UART_SEND
#  define UART_SEND                    (0)
#endif
#ifndef UART_RECV
#  define UART_RECV                    (0)
#endif
#ifndef UART_BAUD
#  define UART_BAUD                    (57600)
#  define UART_BAUD_2X                 (0)
#endif
#if !UART_SEND
#  define UART_SEND_BUFFER_SIZE        (0)
#elif !defined(UART_SEND_BUFFER_SIZE)
#  define UART_SEND_BUFFER_SIZE        (16)
#endif
#if !UART_RECV
#  define UART_RECV_BUFFER_SIZE        (0)
#elif !defined(UART_RECV_BUFFER_SIZE)
#  define UART_RECV_BUFFER_SIZE        (16)
#endif
//===========================================================================
// UART INTERFACE
//===========================================================================
// UART API
VOID     UartInit       ();
BOOL     UartIsSendBusy ();
VOID     UartSendWait   ();
BOOL     UartIsRecvBusy ();
VOID     UartRecvWait   ();
VOID     UartSend       (PCVOID pvData, UI8 cbData);
UI8      UartRecv       (PVOID pvData, UI8 cbData);
VOID     UartSendDelim  (PCVOID pvData, UI8 cbData, BYTE bDelim);
UI8      UartRecvDelim  (PVOID pvData, UI8 cbData, BYTE bDelim);
// UART string API
inline VOID UartSendChar (CHAR ch)
   { UartSend(&ch, 1); }
inline CHAR UartRecvChar ()
   { CHAR ch = 0; UartRecv(&ch, 1); return ch; }
inline VOID UartSendStr (PCSTR psz)
   { UartSend(psz, strlen(psz)); }
inline PSTR UartRecvStr (PSTR psz, UI8 cch)
   { psz[UartRecv(psz, cch)] = '\0'; return psz; }
inline VOID UartSendLine (PCSTR psz)
   { UartSendDelim(psz, strlen(psz), '\n'); }
inline PSTR UartRecvLine (PSTR psz, UI8 cch)
   { psz[UartRecvDelim(psz, cch, '\n')] = '\0'; return psz; }
#endif // __UART_H
