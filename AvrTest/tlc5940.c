//===========================================================================
// Module:  tlc5940.c
// Purpose: TLC4950 PWM driver
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
#include <avr/io.h>
#include <avr/interrupt.h>
//-------------------[      Project Include Files      ]-------------------//
#include "tlc5940.h"
//-------------------[       Module Definitions        ]-------------------//
// driver flags
#define TLC5940_FLAG_UPDATE         0x01
//-------------------[        Module Variables         ]-------------------//
struct 
{
   volatile uint8_t flags;
   volatile union
   {
      uint8_t bytes[24];
      struct __attribute__((__packed__))
      {
         uint16_t ch00 : 12;
         uint16_t ch01 : 12;
         uint16_t ch02 : 12;
         uint16_t ch03 : 12;
         uint16_t ch04 : 12;
         uint16_t ch05 : 12;
         uint16_t ch06 : 12;
         uint16_t ch07 : 12;
         uint16_t ch08 : 12;
         uint16_t ch09 : 12;
         uint16_t ch10 : 12;
         uint16_t ch11 : 12;
         uint16_t ch12 : 12;
         uint16_t ch13 : 12;
         uint16_t ch14 : 12;
         uint16_t ch15 : 12;
      };
   } data;   
} tlc5940;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: tlc5940_init >--------------------------------------
// Purpose:    module initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
void tlc5940_init ()
{
   // initialize the data vector
   memset(&tlc5940, 0, sizeof(tlc5940));

   // 8-bit clock 0, hardware, 409.6 kHz (50Hz servo * 4096 bits PWM * 2 toggles/cycle)
   REGSETBITHI(TCCR0A, COM0A0);                             // toggle OC0A on tick
   REGSETBITHI(TCCR0A, WGM01);                              // CTC value at OCR0A
   REGSETBITHI(TCCR0B, CS00);                               // prescale = 1 (16mHz)
   OCR0A = 38;                                              // reset at 39 ticks

   // 8-bit Clock 2, software, 1kHz
   REGSETBITHI(TCCR2A, WGM21);                              // CTC value at OCR2A
   REGSETBITHI(TCCR2B, CS22);                               // prescale = 64 (250kHz)
   REGSETBITHI(TIMSK2, OCIE2A);                             // enable compare interrupt
   OCR2A = 249;                                             // reset at 250 ticks

   // digital pin setup
   PINSETOUTPUT(PD2);                                       // Arduino 2, TLC5940 23 (BLANK)
   PINSETOUTPUT(PD3);                                       // Arduino 3, TLC5940 25 (SCLK)
   PINSETOUTPUT(PD4);                                       // Arduino 4, TLC5940 26 (SIN)
   PINSETOUTPUT(PD5);                                       // Arduino 5, TLC5940 24 (XLAT)
   PINSETOUTPUT(PD6);                                       // Arduino 6, TLC5940 18 (GSCLK), ATmega328 OC0A

   // set BLANK high
   PINSETHI(PD2);
}
//-----------< FUNCTION: tlc5940_set_duty >----------------------------------
// Purpose:    sets the duty cycle for a PWM channel
// Parameters: channel - TLC5940 output channel (1-16)
//             duty    - duty cycle control (0-4095)
// Returns:    none
//---------------------------------------------------------------------------
void tlc5940_set_duty (uint8_t channel, uint16_t duty)
{
   switch (channel)
   {
      case  0: tlc5940.data.ch00 = duty; break;
      case  1: tlc5940.data.ch01 = duty; break;
      case  2: tlc5940.data.ch02 = duty; break;
      case  3: tlc5940.data.ch03 = duty; break;
      case  4: tlc5940.data.ch04 = duty; break;
      case  5: tlc5940.data.ch05 = duty; break;
      case  6: tlc5940.data.ch06 = duty; break;
      case  7: tlc5940.data.ch07 = duty; break;
      case  8: tlc5940.data.ch08 = duty; break;
      case  9: tlc5940.data.ch09 = duty; break;
      case 10: tlc5940.data.ch10 = duty; break;
      case 11: tlc5940.data.ch11 = duty; break;
      case 12: tlc5940.data.ch12 = duty; break;
      case 13: tlc5940.data.ch13 = duty; break;
      case 14: tlc5940.data.ch14 = duty; break;
      case 15: tlc5940.data.ch15 = duty; break;
   }
   tlc5940.flags |= TLC5940_FLAG_UPDATE;
}
//-----------< INTERRUPT: TIMER2_COMPA_vect >--------------------------------
// Purpose:    responds to the1kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
   static uint16_t ms = 0;
   if (ms % 20 == 0)
   {
      // resync GSCLK and pulse BLANK to restart the cycle
      TCNT0 = 0;
      PINPULSE(PD2);
      // update the 5940's greyscale register
      if (tlc5940.flags & TLC5940_FLAG_UPDATE)
      {
         tlc5940.flags &= ~TLC5940_FLAG_UPDATE;
         // shift in all greyscale bytes
         for (uint8_t i = 0; i < sizeof(tlc5940.data.bytes); i++)
         {
            // shift in the current greyscale byte, MSB first
            for (int8_t j = 7; j >= 0; j--)
            {
               // set SIN to the greyscale bit value and
               // pulse SCLK to shift in the greyscale bit
               PINSET(PD4, tlc5940.data.bytes[sizeof(tlc5940.data.bytes) - i - 1] & (1 << j));
               PINPULSE(PD3);
            }
         }
         // pulse XLAT to latch in the greyscale data
         PINPULSE(PD5);
      }
   }
   ms++;
   if (ms >= 65500)
      ms = 0;
}
