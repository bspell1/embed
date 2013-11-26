//===========================================================================
// Module:  lab.c
// Purpose: AVR test program
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
#include "lab.h"
#include "uart.h"
#include "i2cmast.h"
#include "spimast.h"
#include "mpu6050.h"
#include "tlc5940.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   sei();
   SpiInit();
   PinSetLo(PIN_D4);
   PinSetOutput(PIN_D4);

   TCCR1A |= (1 << COM1A1) | (1 << COM1B1);     // CCR on OC1A/OC1B
   TCCR1A |= (1 << WGM11) | (0 << WGM10);       // fast PWM, freq=ICR1, duty=OCR1A/OCR1B, output OC1A/OC1B
   TCCR1B |= (1 << WGM13) | (1 << WGM12);       // fast PWM, freq=ICR1, duty=OCR1A/OCR1B, output OC1A/OC1B
   TCCR1B |= AvrClk1Scale(64);                  // prescaler of 64, period = 4us
   ICR1    = 5000 - 1;                          // set frequency to 50Hz
#define PWM_MIN 250
#define PWM_MAX 500

   PinSetOutput(PIN_OC1A);
   PinSetOutput(PIN_OC1B);

   for ( ; ; )
   {
      OCR1A = PWM_MIN;
      OCR1B = PWM_MAX;
      PinToggle(PIN_D4);
      _delay_ms(2000);
      OCR1A = PWM_MAX;
      OCR1B = PWM_MIN;
      PinToggle(PIN_D4);
      _delay_ms(2000);
   }
   return 0;
}
