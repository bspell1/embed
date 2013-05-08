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
#include <avr/interrupt.h>
//-------------------[      Project Include Files      ]-------------------//
#include "tlc5940.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
static struct 
{
   UI8            nPinBlank;                             // TLC5940 23 (BLANK)
   UI8            nPinSClk;                              // TLC5940 25 (SCLK)
   UI8            nPinSIn;                               // TLC5940 26 (SIN)
   UI8            nPinXlat;                              // TLC5940 24 (XLAT)
   UI8            nPinGSClk;                             // TLC5940 18 (GSCLK)
   BOOL           bUpdate;                               // update needed indicator
   BYTE           pbGsData[24 * TLC5940_COUNT];          // 5940 greyscale register data
} Tlc5940;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: Tlc5940Init >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID Tlc5940Init (TLC5940_CONFIG* pConfig)
{
   // initialize the data vector
   memset(&Tlc5940, 0, sizeof(Tlc5940));
   Tlc5940.nPinBlank = pConfig->nPinBlank;
   Tlc5940.nPinSClk  = pConfig->nPinSClk;
   Tlc5940.nPinSClk  = pConfig->nPinSClk;
   Tlc5940.nPinXlat  = pConfig->nPinXlat;
   Tlc5940.nPinGSClk = pConfig->nPinGSClk;
   // 8-bit clock 0, hardware, 409.6 kHz (50Hz servo * 4096 bits PWM * 2 toggles/cycle)
   REG_SET_HI(TCCR0A, COM0A0);                           // toggle OC0A on tick
   REG_SET_HI(TCCR0A, WGM01);                            // CTC value at OCR0A
   REG_SET_HI(TCCR0B, CS00);                             // prescale = 1 (16mHz)
   OCR0A = F_CPU / 409600 - 1;                           // reset at 39 ticks
   // 8-bit clock 2, software, 10kHz
   REG_SET_HI(TCCR2A, WGM21);                            // CTC mode, compare at OCR2A
   REG_SET_HI(TCCR2B, CS22);                             // prescale = 64 (250kHz)
   REG_SET_HI(TIMSK2, OCIE2A);                           // enable compare interrupt A
   OCR2A = F_CPU / 64 / 10000 - 1;                       // reset OC2A at 25 ticks for 10kHz
   // digital pin setup
   PIN_SET_OUTPUT(Tlc5940.nPinBlank);                           
   PIN_SET_OUTPUT(Tlc5940.nPinSClk);                            
   PIN_SET_OUTPUT(Tlc5940.nPinSIn);                             
   PIN_SET_OUTPUT(Tlc5940.nPinXlat);                            
   PIN_SET_OUTPUT(Tlc5940.nPinGSClk);                         
   // set BLANK high
   PIN_SET_HI(Tlc5940.nPinBlank);
}
//-----------< FUNCTION: Tlc5940GetDuty >------------------------------------
// Purpose:    gets the duty cycle for a PWM channel
// Parameters: nModule  - TLC5940 number (distance) in the daisy chain
//             nChannel - TLC5940 output channel (0-15)
// Returns:    the current duty cycle control value for the channel
//---------------------------------------------------------------------------
UI16 Tlc5940GetDuty (UI8 nModule, UI8 nChannel)
{
   // decode the control value from the data buffer
   // . the module offset identifies the 
   // . each control value is 12 bits wide, stored big endian
   // . channel values are stored in reverse order (channel 15 first)
   // . output channels are active low, so subtract from 4095
   UI8  cbModule = (TLC5940_COUNT - nModule) * 24;
   UI8  cbOffset = 23 - nChannel * 3 / 2 - 1;
   UI8* pb       = Tlc5940.pbGsData + cbModule + cbOffset;
   UI16 nLoDuty  = (nChannel % 2 == 0) ? 
      ((UI16)(pb[0] & 0x0F) << 8) | pb[1] : 
      ((UI16)pb[0] << 4) | (pb[1] >> 4);
   return 4095 - nLoDuty;
}
//-----------< FUNCTION: Tlc5940SetDuty >------------------------------------
// Purpose:    sets the duty cycle for a PWM channel
// Parameters: nModule  - TLC5940 number (distance) in the daisy chain
//             nChannel - TLC5940 output channel (0-15)
//             nDuty    - duty cycle control (0-4095)
// Returns:    none
//---------------------------------------------------------------------------
VOID Tlc5940SetDuty (UI8 nModule, UI8 nChannel, UI16 nDuty)
{
   // decode the control value from the data buffer
   // . each control value is 12 bits wide, stored big endian
   // . channel values are stored in reverse order (channel 15 first)
   // . output channels are active low, so subtract from 4095
   UI8  cbModule = (TLC5940_COUNT - nModule) * 24;
   UI8  cbOffset = 23 - nChannel * 3 / 2 - 1;
   UI8* pb       = Tlc5940.pbGsData + cbModule + cbOffset;
   UI16 nLoDuty  = 4095 - nDuty;
   if (nChannel % 2 == 0)
   {
      pb[0] = (pb[0] & 0xF0) | (nLoDuty >> 8);   // high nibble at low nibble of offset
      pb[1] = nLoDuty;                           // low byte at next offset
   }
   else
   {
      pb[0] = nLoDuty >> 4;                      // high byte at offset
      pb[1] = (pb[1] & 0x0F) | (nLoDuty << 4);   // low nibble at high nibble of next offset
   }
   Tlc5940.bUpdate = TRUE;
}
//-----------< INTERRUPT: TIMER2_COMPA_vect >--------------------------------
// Purpose:    responds to 10kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
   // servo cycle is 20ms
   static UI16 fms = 0;
   if (fms++ % 200 == 0)
   {
      // uninterruptible phase
      // resync GSCLK and pulse BLANK to start the next PWM cycle
      TCNT0 = 0;
      PIN_PULSE(Tlc5940.nPinBlank);
      // update the 5940's greyscale register if requested
      if (Tlc5940.bUpdate)
      {
         Tlc5940.bUpdate = FALSE;
         // interuptible phase
         // shift in greyscale bytes, MSB first
         sei();
         for (UI8 i = 0; i < sizeof(Tlc5940.pbGsData); i++)
         {
            // shift in the current greyscale byte, MSB first
            for (I8 j = 7; j >= 0; j--)
            {
               // set SIN to the greyscale bit value and
               // pulse SCLK to shift in the greyscale bit
               PIN_SET(Tlc5940.nPinSIn, BIT_TEST(Tlc5940.pbGsData[i], j));
               PIN_PULSE(Tlc5940.nPinSClk);
            }
         }
         // pulse XLAT to latch in the greyscale data
         PIN_PULSE(Tlc5940.nPinXlat);
      }
      fms = 0;
   }
}
