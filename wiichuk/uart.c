//===========================================================================
// Module:  uart.c
// Purpose: AVR UART master driver
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
#include "uart.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: UartInit >------------------------------------------
// Purpose:    UART interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID UartInit ()
{
   UBRR0  = (((F_CPU / (UART_BAUD * 16))) - 1);       // set base baud rate
   UCSR0A = (UART_BAUD_2X << U2X0);                   // set 2x multiplier
   UCSR0C = (3<<UCSZ00) |                             // set 8 data bits
            (0<<USBS0) |                              // set 1 stop bit
            (0<<UPM00) | (0<<UPM01);                  // set 0 parity bits
   if (UART_SEND)                                     // enable TX
      RegSetHi(UCSR0B, TXEN0);
   if (UART_RECV)                                     // enable RX
      RegSetHi(UCSR0B, RXEN0);
   if (UART_INTERRUPT)                                // enable RX interrupts
      RegSetHi(UCSR0B, RXCIE0);
}
//-----------< FUNCTION: UartIsSendBusy >------------------------------------
// Purpose:    polls the UART send busy state
// Parameters: none
// Returns:    TRUE if there is a transmit in progress
//             FALSE otherwise
//---------------------------------------------------------------------------
BOOL UartIsSendBusy ()
{
   return !RegGet(UCSR0A, UDRE0);
}
//-----------< FUNCTION: UartSendWait >--------------------------------------
// Purpose:    waits for the UART transmit register to become available
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendWait ()
{
   while (UartIsSendBusy())
      ;
}
//-----------< FUNCTION: UartIsRecvBusy >------------------------------------
// Purpose:    polls the UART receive busy state
// Parameters: none
// Returns:    TRUE if there is a transmit in progress
//             FALSE otherwise
//---------------------------------------------------------------------------
BOOL UartIsRecvBusy ()
{
   return !RegGet(UCSR0A, RXC0);
}
//-----------< FUNCTION: UartRecvWait >--------------------------------------
// Purpose:    waits for the UART receive register to become available
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID UartRecvWait ()
{
   while (UartIsRecvBusy())
      ;
}
//-----------< FUNCTION: UartSend >------------------------------------------
// Purpose:    sends a byte on the UART interface
// Parameters: b - the byte to send
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSend (BYTE b)
{
   UartSendWait();
   UDR0 = b;
}
//-----------< FUNCTION: UartRecv >------------------------------------------
// Purpose:    receives a byte on the UART interface
// Parameters: none
// Returns:    the byte received
//---------------------------------------------------------------------------
BYTE UartRecv ()
{
   UartRecvWait();
   return UDR0;
}
