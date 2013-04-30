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
   volatile uint8_t  flags;
   uint8_t           data[24];
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
//-----------< FUNCTION: tlc5940_get_duty >----------------------------------
// Purpose:    gets the duty cycle for a PWM channel
// Parameters: channel - TLC5940 output channel (1-16)
// Returns:    the current duty cycle control value for the channel
//---------------------------------------------------------------------------
uint16_t tlc5940_get_duty (uint8_t channel)
{
   // decode the control value from the data buffer
   // . each control value is 12 bits wide, stored big endian
   // . channel values are stored in reverse order (channel 15 first)
   uint8_t offset = 23 - channel * 3 / 2 - 1;
   uint8_t* pb = tlc5940.data + offset;
   return (channel % 2 == 0) ? 
      ((uint16_t)(pb[0] & 0x0F) << 8) | pb[1] : 
      ((uint16_t)pb[0] << 4) | (pb[1] >> 4);
}
//-----------< FUNCTION: tlc5940_set_duty >----------------------------------
// Purpose:    sets the duty cycle for a PWM channel
// Parameters: channel - TLC5940 output channel (1-16)
//             duty    - duty cycle control (0-4095)
// Returns:    none
//---------------------------------------------------------------------------
void tlc5940_set_duty (uint8_t channel, uint16_t duty)
{
   // decode the control value from the data buffer
   // . each control value is 12 bits wide, stored big endian
   // . channel values are stored in reverse order (channel 15 first)
   uint8_t offset = 23 - channel * 3 / 2 - 1;
   uint8_t* pb = tlc5940.data + offset;
   if (channel % 2 == 0)
   {
      pb[0] = (pb[0] & 0xF0) | (duty >> 8);     // store high nibble at offset low nibble
      pb[1] = duty;                             // store low byte at next offset
   }
   else
   {
      pb[0] = duty >> 4;                        // store high byte at offset
      pb[1] = (pb[1] & 0x0F) | (duty << 4);     // store low nibble at next offset high nibble
   }
   tlc5940.flags |= TLC5940_FLAG_UPDATE;
}
//-----------< INTERRUPT: TIMER2_COMPA_vect >--------------------------------
// Purpose:    responds to 1kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
   static uint16_t ms = 0;
   if (ms % 20 == 0)
   {
      // resync GSCLK and pulse BLANK to start the next PWM cycle
      TCNT0 = 0;
      PINPULSE(PD2);
      // update the 5940's greyscale register if requested
      if (tlc5940.flags & TLC5940_FLAG_UPDATE)
      {
         tlc5940.flags &= ~TLC5940_FLAG_UPDATE;
         // shift in greyscale bytes, MSB first
         for (uint8_t i = 0; i < sizeof(tlc5940.data); i++)
         {
            // shift in the current greyscale byte, MSB first
            for (int8_t j = 7; j >= 0; j--)
            {
               // set SIN to the greyscale bit value and
               // pulse SCLK to shift in the greyscale bit
               PINSET(PD4, BITTEST(tlc5940.data[i], j));
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
