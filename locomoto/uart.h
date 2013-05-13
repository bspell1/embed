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
// . UART_SEND:      enable UART send
// . UART_RECV:      enable UART receive
// . UART_INTERRUPT: enable UART interrupts
// . UART_BAUD:      sets the base UART baud rate
// . UART_BAUD_2X:   sets the UART 2x baud rate multiplier
//===========================================================================
#ifndef UART_SEND
#  define UART_SEND                 (TRUE)
#endif
#ifndef UART_RECV
#  define UART_RECV                 (TRUE)
#endif
#ifndef UART_INTERRUPT
#  define UART_INTERRUPT            (TRUE)
#endif
#ifndef UART_BAUD
#  define UART_BAUD                 (57600)
#  define UART_BAUD_2X              (FALSE)
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
VOID     UartSend       (BYTE b);
BYTE     UartRecv       ();
#endif // __UART_H
