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
#include "i2cmast.h"
#include "tlc5940.h"
#include "sx1509.h"
#include "proto.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
static HSX1509 g_h1509;
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

   I2cInit(NULL);
   Tlc5940Init();
   ProtoInit();

   g_h1509 = SX1509Init(0x3E);

   sei();

   SX1509SetDirOutput(g_h1509, 0);                      // set pin to output
   SX1509SetDataLo(g_h1509, 0);                      // set pin to output

   for ( ; ; )
   {
   }

   #if 0
   SX1509SetClockSourceInternal(g_h1509);               // internal oscillator
   SX1509SetMiscPwmFrequency(g_h1509, 1);               // set PWM frequency to 1mHz

   SX1509SetInputDisableHi(g_h1509, 4);                 // disable input
   SX1509SetPullUpLo(g_h1509, 4);                       // disable pull-up
   SX1509SetOpenDrainHi(g_h1509, 4);                    // enable open drain
   SX1509SetDirOutput(g_h1509, 4);                      // set pin to output
   SX1509SetPwmEnableHi(g_h1509, 4);                    // enable PWM

   SX1509Set8(g_h1509, SX1509_REG_TON4, 15);
   SX1509Set8(g_h1509, SX1509_REG_TRISE4, 15);
   SX1509Set8(g_h1509, SX1509_REG_TFALL4, 15);
   SX1509Set8(g_h1509, SX1509_REG_OFF4, 15 << 3);

   SX1509SetDataLo(g_h1509, 4);                         // set pin 0 lo to activate PWM

   SX1509SetDirOutput(g_h1509, 0);                      // set pin 0 output

/*
   I8 dir = 1;
   UI8 pwm = 0;
   for ( ; ; )
   {
      SX1509Set8(g_h1509, SX1509_REG_ION4, pwm);
      pwm += dir;
      if (pwm == 0 || pwm == 255)
         dir *= -1;
      _delay_ms(5);
   }
*/
   #endif

   #if 0
   UI16 min = 90;
   UI16 max = 440;
   UI16 mul = 1;
   UI16 duty = min;
   I8 dir = 1;
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
