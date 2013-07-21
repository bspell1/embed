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
#include "debug.h"
#include "uart.h"
#include "hcsr04.h"
#include "shiftreg.h"
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
   // initialize the module
   PinSetOutput(PIN_ARDUINO_LED);
   PinSetLo(PIN_ARDUINO_LED);

   // temperature sensor input config
   ADMUX  |= (1<<REFS1) | (1<<REFS0) |                // 1.1v internal reference
             (1<<MUX3);                               // ADC source 8 (temperature sensor)
   ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);    // 150kHz ADC clock for 10-bits

   // start up the ADC
   RegSetHi(ADCSRA, ADEN);

   for ( ; ; )
   {
      _delay_ms(1000);
      // run the next conversion
      RegSetHi(ADCSRA, ADSC);
      while (BitTest(ADCSRA, ADSC))
         ;
      // send the conversion results
      UI8 l = ADCL;
      UI8 h = ADCH;
      UartSendByte(h);
      UartSendByte(l);
   }
   return 0;
}
