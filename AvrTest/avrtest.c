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
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
//-------------------[      Project Include Files      ]-------------------//
#include "avrtest.h"
#include "tlc5940.h"
#include "i2cmast.h"
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
   cli();

   PIN_SET_OUTPUT(PIN_ARDUINO_LED);

   PIN_SET_LO(PIN_ARDUINO_LED);

   Tlc5940Init();
   I2cInit(NULL);

   //PIN_SET_HI(PIN_SDA);
   //PIN_SET_HI(PIN_SCL);

   sei();

   // SX1509 addresses: 0x3E, 0x3F, 0x70, 0x71
   UI8 addr = 0x3E;
   BYTE RegDir[]  = { 0x0F, 0x00 }; // set all pins to output
   BYTE RegData[] = { 0x11, 0x00 }; // initialize outputs to low
   I2cSend(addr, RegDir, sizeof(RegDir));
   for ( ; ; )
   {
      RegData[1] = 0xFF;
      I2cSend(addr, RegData, sizeof(RegData));
      _delay_ms(1000);
      RegData[1] = 0x00;
      I2cSend(addr, RegData, sizeof(RegData));
      _delay_ms(1000);
   }

   #if 0
   UI16 min = 90;
   UI16 max = 440;
   UI16 mul = 1;
   UI16 duty = min;
   UI8 dir = 1;
   for ( ; ; )
   {
      Tlc5940SetDuty(0, duty);
      duty += dir * mul;
      if (duty < min)
         duty = min;
      else if (duty > max)
         duty = max;
      if (duty == min || duty == max)
      {
         dir *= -1;
         _delay_ms(2000);
      }
      _delay_ms(1);
   }
   #endif
   return 0;
}
