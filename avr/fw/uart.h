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
#include <stdio.h>
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
// configuration properties
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
#  define UART_SEND_BUFFER_SIZE        (1)
#elif !defined(UART_SEND_BUFFER_SIZE)
#  define UART_SEND_BUFFER_SIZE        (16)
#endif
#if !UART_RECV
#  define UART_RECV_BUFFER_SIZE        (1)
#elif !defined(UART_RECV_BUFFER_SIZE)
#  define UART_RECV_BUFFER_SIZE        (16)
#endif
// UART callback
typedef VOID (*UART_CALLBACK) (BYTE bData);
// configuration structure
typedef struct tagUartConfig
{
   UART_CALLBACK  pfnOnSend;           // send complete callback
   UART_CALLBACK  pfnOnRecv;           // receive complete callback
} UART_CONFIG, *PUART_CONFIG;
//===========================================================================
// UART INTERFACE
//===========================================================================
// UART API
VOID     UartInit       (PUART_CONFIG pConfig);
UI8      UartSendReady  ();
VOID     UartSend       (PCVOID pvData, UI8 cbData);
VOID     UartSendDelim  (PCVOID pvData, UI8 cbData, BYTE bDelim);
VOID     UartSendStr    (PCSTR psz, ...);
VOID     UartSendStrV   (PCSTR psz, va_list args);
VOID     UartSendLine   (PCSTR psz, ...);
VOID     UartSendLineV  (PCSTR psz, va_list args);
UI8      UartRecvReady  ();
UI8      UartRecv       (PVOID pvData, UI8 cbData);
// UART helpers
inline VOID UartSendByte (BYTE b)
   { UartSend(&b, 1); }
inline VOID UartSendChar (CHAR ch)
   { UartSend(&ch, 1); }
// UART tracing
#define UartTrace UartSendStr
#endif // __UART_H
