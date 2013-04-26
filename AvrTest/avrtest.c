//===========================================================================
// Module:  avrtest.c
// Purpose: AVR microprocessor test laboratory
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
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
//-------------------[       Module Definitions        ]-------------------//
#define USART_BAUDRATE 57600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16))) - 1)
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
void uart_init ()
{
   UBRR0H = BAUD_PRESCALE >> 8;                       // set baud rate
   UBRR0L = BAUD_PRESCALE;
   UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);    // enable TX/RX
}
void uart_write (uint8_t c)
{
   while (!(UCSR0A & (1<<UDRE0)));  // wait until data register empty
   UDR0 = c;
}
uint8_t uart_read ()
{
   while (!(UCSR0A & (1<<RXC0)));   // wait until data register full
   return (uint8_t)UDR0;
}
ISR(USART_RX_vect)
{
   uint8_t data = uart_read();
   uart_write(data);
}
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int  main ()
{
   uart_init();

   DDRB |= 1 << PB5;

   TCCR1A = (1 << WGM01);                             // toggle to COM1A0
   TCCR1B |= (1 << WGM12);                            // set CTC mode
   TCCR1B |= (1 << CS11);                             // prescale = 64
   TIMSK1 |= (1 << TOIE1);                            // enable overflow event
   OCR1A = 25599;                                     // compare at 25599
   TCNT1 = 65536 - 4096;                              // overflow in 4096 ticks

   sei();

   for ( ; ; )
   {

   }
   return 0;
}
ISR(TIMER0_OVF_vect) {
   PORTB ^= (1 << PB5);
   TCNT1 = 65536 - 4096;
}
