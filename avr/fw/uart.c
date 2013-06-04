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
#include "fifo.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
// send/receive queues
DECLARE_FIFO(g_pSendFifo, UART_SEND_BUFFER_SIZE);
DECLARE_FIFO(g_pRecvFifo, UART_RECV_BUFFER_SIZE);
// callback functions
static UART_CALLBACK g_pfnOnSend = NULL;
static UART_CALLBACK g_pfnOnRecv = NULL;
//-----------< FUNCTION: UartInit >------------------------------------------
// Purpose:    UART interface initialization
// Parameters: pConfig - configuration structure
// Returns:    none
//---------------------------------------------------------------------------
VOID UartInit (PUART_CONFIG pConfig)
{
   g_pfnOnSend = pConfig->pfnOnSend;
   g_pfnOnRecv = pConfig->pfnOnRecv;
#if (UART_BAUD_2X)                                  // set base baud rate
   UBRR0  = (UI16)((double)F_CPU / ((double)UART_BAUD * 8) - 1);
#else
   UBRR0  = (UI16)((double)F_CPU / ((double)UART_BAUD * 16) - 1);
#endif
   UCSR0A = (UART_BAUD_2X << U2X0);                   // set 2x multiplier
   UCSR0B = (UART_SEND << TXEN0) |                    // enable TX
            (UART_RECV << RXEN0) |                    // enable RX
            (UART_RECV << RXCIE0);                    // enable RX interrupt
   UCSR0C = (3<<UCSZ00) |                             // set 8 data bits
            (0<<USBS0) |                              // set 1 stop bit
            (0<<UPM00) | (0<<UPM01);                  // set 0 parity bits
}
//-----------< FUNCTION: UartSendReady >-------------------------------------
// Purpose:    retrieves the number of bytes waiting to be sent
// Parameters: none
// Returns:    the number of bytes in the send FIFO
//---------------------------------------------------------------------------
UI8 UartSendReady ()
{
   return FifoCount(g_pSendFifo);
}
//-----------< FUNCTION: UartSend >------------------------------------------
// Purpose:    sends a message on the UART interface
// Parameters: pbData - the message to send
//             cbData - the number of bytes to send
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSend (PCVOID pvData, UI8 cbData)
{
   // transfer the entire buffer, spinning when the FIFO becomes full
   PCBYTE pbData = (PCBYTE)pvData;
   while (cbData > 0)
   {
      UI8 cbSent = 0;
      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      {
         BOOL bStart = !RegGet(UCSR0B, UDRIE0);
         // fill the send FIFO
         cbSent = MIN(cbData, FifoSize(g_pSendFifo) - FifoCount(g_pSendFifo));
         FifoWriteBlock(g_pSendFifo, pbData, cbSent);
         // if the FIFO was empty, start the UART transfer
         if (cbSent > 0 && bStart)
         {
            UDR0 = FifoRead(g_pSendFifo);
            RegSetHi(UCSR0B, UDRIE0);
         }
      }
      pbData += cbSent;
      cbData -= cbSent;
   }
}
//-----------< FUNCTION: UartSendDelim >-------------------------------------
// Purpose:    sends a message, terminated with a delimiter
// Parameters: pbData - the message to send
//             cbData - the number of bytes to send
//             bDelim - the message delimiter
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendDelim (PCVOID pvData, UI8 cbData, BYTE bDelim)
{
   UartSend(pvData, cbData);
   UartSend(&bDelim, 1);
}
//-----------< FUNCTION: UartSendStr >---------------------------------------
// Purpose:    sends a formatted string over UART
// Parameters: psz - the format string
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendStr (PCSTR psz, ...)
{
   va_list args; va_start(args, psz);
   UartSendStrV(psz, args);
   va_end(args);
}
//-----------< FUNCTION: UartSendStrV >--------------------------------------
// Purpose:    sends a formatted string over UART
// Parameters: psz  - the format string
//             args - variable format string arguments
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendStrV (PCSTR psz, va_list args)
{
   UI8 cchBuffer = vsnprintf(NULL, 0, psz, args) + 1;
   CHAR szBuffer[cchBuffer + 1];
   vsnprintf(szBuffer, cchBuffer, psz, args);
   szBuffer[cchBuffer] = '\0';
   UartSend(szBuffer, cchBuffer);
}
//-----------< FUNCTION: UartSendLine >--------------------------------------
// Purpose:    sends a formatted, line-terminated string over UART
// Parameters: psz - the format string
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendLine (PCSTR psz, ...)
{
   va_list args; va_start(args, psz);
   UartSendLineV(psz, args);
   va_end(args);
}
//-----------< FUNCTION: UartSendLineV >-------------------------------------
// Purpose:    sends a formatted, line-terminated string over UART
// Parameters: psz  - the format string
//             args - variable format string arguments
// Returns:    none
//---------------------------------------------------------------------------
VOID UartSendLineV (PCSTR psz, va_list args)
{
   UI8 cchBuffer = vsnprintf(NULL, 0, psz, args) + 1;
   CHAR szBuffer[cchBuffer + 1];
   vsnprintf(szBuffer, cchBuffer, psz, args);
   szBuffer[cchBuffer] = '\0';
   UartSendDelim(szBuffer, cchBuffer, '\n');
}
//-----------< FUNCTION: UartRecvReady >-------------------------------------
// Purpose:    retrieves the number of bytes available to receive
// Parameters: none
// Returns:    the number of bytes in the receive FIFO
//---------------------------------------------------------------------------
UI8 UartRecvReady ()
{
   return FifoCount(g_pRecvFifo);
}
//-----------< FUNCTION: UartRecv >------------------------------------------
// Purpose:    receives a message on the UART interface
// Parameters: pbData - return the message via here
//             cbData - the number of bytes to receive
// Returns:    the actual number of bytes received
//---------------------------------------------------------------------------
UI8 UartRecv (PVOID pvData, UI8 cbData)
{
   UI8 cbRecv = 0;
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
      cbRecv = FifoReadBlock(g_pRecvFifo, pvData, cbData);
   return cbRecv;
}
//-----------< INTERRUPT: USART_UDRE_vect >----------------------------------
// Purpose:    responds to UART data register empty complete events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(USART_UDRE_vect)
{
   if (FifoIsEmpty(g_pSendFifo))
      RegSetLo(UCSR0B, UDRIE0);
   else
   {
      BYTE bSend = FifoRead(g_pSendFifo);;
      UDR0 = bSend;
      if (g_pfnOnSend != NULL)
         g_pfnOnSend(bSend);
   }
}
//-----------< INTERRUPT: USART_RX_vect >------------------------------------
// Purpose:    responds to UART receive complete events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(USART_RX_vect)
{
   BYTE bRecv = UDR0;
   FifoWrite(g_pRecvFifo, bRecv);
   if (g_pfnOnRecv != NULL)
      g_pfnOnRecv(bRecv);
}
