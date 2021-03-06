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
#include <math.h>
//-------------------[      Project Include Files      ]-------------------//
#include "lab.h"
#include "uart.h"
#include "i2cmast.h"
#include "spimast.h"
#include "mpu6050.h"
#include "nrf24.h"
#include "shiftreg.h"
#include "sevenseg.h"
//-------------------[       Module Definitions        ]-------------------//
#define PIN_LED   PIN_D6
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
static VOID LabInit  ();
static VOID LabRun   ();
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   LabInit();
   for ( ; ; )
      LabRun();
   return 0;
}
//-----------< FUNCTION: LabInit >-------------------------------------------
// Purpose:    lab program initialiization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LabInit ()
{
   sei();

   PinSetOutput(PIN_LED);

   UartInit(&(UART_CONFIG) { 0, });

   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_D2,
         .nLatchPin = PIN_D3,
         .nDataPin  = PIN_D4
      }
   );

   SevenSegInit(
      &(SEVENSEG_CONFIG)
      {
         .nSROffset = 0,
         .pnMuxPins = (UI8[2]){ PIN_D7, PIN_B0 },
         .fReverse  = FALSE
      }
   );

   SevenSegSet16(1234);

   /*// 1kHz clock 0
   TCCR0A = BitMask(COM0A0) | BitMask(WGM01);
   TCCR0B = AvrClk0Scale(64);
   OCR0A  = AvrClkTop(64, 1000);
   */

   /**/// 1kHz clock 2, interrupt
   TCCR2A = BitMask(WGM21);
   TCCR2B = AvrClk2Scale(64);
   TIMSK2 = BitMask(OCIE2A);
   OCR2A  = AvrClkTop(64, 1000);
   /**/
}
//-----------< FUNCTION: LabRun >--------------------------------------------
// Purpose:    lab main loop
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LabRun ()
{
}
//-----------< INTERRUPT: TIMER2_COMPA_vect >--------------------------------
// Purpose:    handles timer 2 ticks
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPA_vect)
{
   static UI16 g_c = 0;
   static BOOL g_b = FALSE;
   if (g_c++ % 250 == 0) {
      SevenSegStrobe();
      if (g_b)
         PinToggle(PIN_LED);
      g_b = !g_b;
   }
}
