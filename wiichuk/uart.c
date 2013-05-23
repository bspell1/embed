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
#include <avr/interrupt.h>
//-------------------[      Project Include Files      ]-------------------//
#include "uart.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
static volatile BYTE g_pbSend[UART_SEND_BUFFER_SIZE]; // send buffer
static volatile BYTE g_pbRecv[UART_RECV_BUFFER_SIZE]; // receive buffer
static volatile UI8  g_cbSend = 0;                    // send buffer length
static volatile UI8  g_cbRecv = 0;                    // receive buffer length
//-----------< FUNCTION: UartInit >------------------------------------------
// Purpose:    UART interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID UartInit ()
{
   UBRR0  = (((F_CPU / (UART_BAUD * 16))) - 1);       // set base baud rate
   UCSR0A = (UART_BAUD_2X << U2X0);                   // set 2x multiplier
   UCSR0B = (UART_SEND << TXEN0) |                    // enable TX
            (UART_RECV << RXEN0);                     // enable RX
   UCSR0C = (3<<UCSZ00) |                             // set 8 data bits
            (0<<USBS0) |                              // set 1 stop bit
            (0<<UPM00) | (0<<UPM01);                  // set 0 parity bits
}
//-----------< FUNCTION: UartIsSendBusy >------------------------------------
// Purpose:    polls the UART send busy state
// Parameters: none
// Returns:    TRUE if there is a transmit in progress
//             FALSE otherwise
//---------------------------------------------------------------------------
BOOL UartIsSendBusy ()
{
   return !RegGet(UCSR0B, TXCIE0);
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
   return !RegGet(UCSR0B, RXCIE0);
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
#if UART_SEND
//-----------< FUNCTION: UartSend >------------------------------------------
// Purpose:    sends a message on the UART interface
// Parameters: pbData - the message to send
//             cbData - the number of bytes to send
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSend (PCVOID pvData, UI8 cbData)
{
   // lock the UART module
   UartSendWait();
   // set up UART state and transfer the send buffer
   cbData = MIN(cbData, UART_SEND_BUFFER_SIZE);
   memcpy((PBYTE)g_pbSend, pvData, cbData);
   g_cbSend = cbData;
   // start the transmission
   if (cbData > 0)
   {
      RegSetHi(UCSR0B, TXCIE0);
      UDR0 = g_pbSend[0];
   }
}
//-----------< INTERRUPT: USART_TX_vect >------------------------------------
// Purpose:    responds to UART trasnmit complete events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(USART_TX_vect)
{
   static UI8 g_cbTx = 0;
   // transmit the next byte if not done
   if (++g_cbTx < g_cbSend)
      UDR0 = g_pbSend[g_cbTx];
   else
   {
      // complete the transmission and disable the interrupt
      g_cbTx = g_cbSend = 0;
      RegSetLo(UCSR0B, TXCIE0);
   }
}
#endif
#if UART_RECV
//-----------< FUNCTION: UartRecv >------------------------------------------
// Purpose:    receives a message on the UART interface
// Parameters: pbData - return the message via here
//             cbData - the number of bytes to receive
// Returns:    the actual number of bytes received
//---------------------------------------------------------------------------
UI8 UartRecv (PVOID pvData, UI8 cbData)
{
   // lock the UART module
   UartRecvWait();
   // set up UART state for the transfer
   cbData = MIN(cbData, UART_RECV_BUFFER_SIZE);
   g_cbRecv = cbData;
   if (cbData > 0)
   {
      // start the transfer and wait for it to complete
      RegSetHi(UCSR0B, RXCIE0);
      UartRecvWait();
   }
   return cbData;
}
//-----------< INTERRUPT: USART_RX_vect >------------------------------------
// Purpose:    responds to UART receive complete events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(USART_RX_vect)
{
   static UI8 g_cbRx = 0;
   // read the incoming byte
   g_pbRecv[g_cbRx] = UDR0;
   if (++g_cbRx >= g_cbRecv)
   {
      // complete the receive and disable the interrupt
      g_cbRx = g_cbRecv = 0;
      RegSetLo(UCSR0B, RXCIE0);
   }
}
#endif
