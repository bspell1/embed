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
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
//-------------------[       Module Definitions        ]-------------------//
#define TLC5940_FLAG_UPDATE 0x01
struct 
{
   uint8_t flags;
   union
   {
      uint8_t bytes[24];
      struct __attribute__((__packed__))
      {
         uint16_t ch15 : 12;
         uint16_t ch14 : 12;
         uint16_t ch13 : 12;
         uint16_t ch12 : 12;
         uint16_t ch11 : 12;
         uint16_t ch10 : 12;
         uint16_t ch09 : 12;
         uint16_t ch08 : 12;
         uint16_t ch07 : 12;
         uint16_t ch06 : 12;
         uint16_t ch05 : 12;
         uint16_t ch04 : 12;
         uint16_t ch03 : 12;
         uint16_t ch02 : 12;
         uint16_t ch01 : 12;
         uint16_t ch00 : 12;
      };
   } data;   
} tlc5940;
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int  main ()
{
   cli();

   memset(&tlc5940, 0, sizeof(tlc5940));

   // 8-bit clock 0, hardware, 409.6 kHz (50Hz servo * 4096 bits PWM * 2 toggles/cycle)
   TCCR0A |= (1 << COM0A0);                                 // toggle OC0A on tick
   TCCR0A |= (1 << WGM01);                                  // CTC value at OCR0A
   TCCR0B |= (0 << CS02) | (0 << CS01) | (1 << CS00);       // prescale = 1 (16Mhz)
   OCR0A = 38;                                              // reset at 39 ticks
 
   // 8-bit Clock 2, software, 1000 ticks/sec
   TCCR2A |= (1 << WGM21);                                  // CTC value at OCR2A
   TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);       // prescale = 64 (250kHz)
   TIMSK2 |= (1 << OCIE2A);                                 // enable compare event
   OCR2A = 249;                                             // reset at 250 ticks
 
   sei();

   // digital pin setup
   DDRB |= (1 << PB5);                                      // Arduino 13/LED

   DDRD |= (1 << PD2);                                      // Arduino 2, TLC5940 BLANK
   DDRD |= (1 << PD3);                                      // Arduino 3, TLC5940 SCLK
   DDRD |= (1 << PD4);                                      // Arduino 4, TLC5940 SIN
   DDRD |= (1 << PD5);                                      // Arduino 5, TLC5940 XLAT
   DDRD |= (1 << PD6);                                      // Arduino 6, TLC5940 GSCLK, ATmega328 OC0A

   PORTD |= (1 << PD2);                                     // set BLANK high

   // set channel 0 to duty cycle of 1.5ms
   tlc5940.data.ch00 = 308;
   tlc5940.flags |= TLC5940_FLAG_UPDATE;

   for ( ; ; )
   {
      sleep_cpu();
   }
   return 0;
}
ISR(TIMER2_COMPA_vect)
{
   static uint16_t ms = 0;
   if (ms % 20 == 0)
   {
      // pulse BLANK to restart cycle
      PORTD &= ~(1 << PD2);
      PORTD |= (1 << PD2);
      // update the greyscale values
      if (tlc5940.flags & TLC5940_FLAG_UPDATE)
      {
         tlc5940.flags &= ~TLC5940_FLAG_UPDATE;
         // shift in all greyscale bytes
         for (uint8_t i = 0; i < sizeof(tlc5940.data.bytes); i++)
         {
            // shift in the current greyscale byte, MSB first
            for (int8_t j = 7; j >= 0; j--)
            {
               // set SIN to the greyscale bit value
               if (tlc5940.data.bytes[i] & (1 << j))
                  PORTD |= (1 << PD4);
               else
                  PORTD &= ~(1 << PD4);
               // pulse SCLK to shift in the greyscale bit
               PORTD |= (1 << PD3);
               PORTD &= ~(1 << PD3);
            }
         }
         // pulse XLAT to latch in the greyscale data
         PORTD |= (1 << PD5);
         PORTD &= ~(1 << PD5);
      }
   }
   ms++;
   if (ms >= 65500)
      ms = 0;
}
