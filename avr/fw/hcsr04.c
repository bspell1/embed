//===========================================================================
// Module:  hcsr04.c
// Purpose: HCSR04 ultrasonic ranging module driver
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
#include "hcsr04.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
// per-module state
static volatile struct
{
   UI8   nTrigPin;               // trigger (output) pin
   UI8   nEchoPin;               // echo (input) pin
   UI16  nReading;               // last range module reading
} g_pModules[HCSR04_COUNT];
// current module being read
static volatile UI8 g_nModule = HCSR04_COUNT - 1;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: HandlePinChange >-----------------------------------
// Purpose:    handles a ranging module pin change event
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID HandlePinChange ()
{
   static volatile UI16 g_nStart;
   // sample the current 250kHz timer value
   UI16 nTime = TCNT1;
   // if the echo pin is hi, we are starting, otherwise stopping
   if (PinRead(g_pModules[g_nModule].nEchoPin))
      g_nStart = nTime;
   else
   {
      // compute the duration, in 4us units
      UI16 nDuration = nTime >= g_nStart ? 
         nTime - g_nStart : 
         UI16_MAX - g_nStart + nTime + 1;
      // convert the duration to microseconds and record
      g_pModules[g_nModule].nReading = nDuration * 4;
   }
}
//-----------< FUNCTION: HCSR04Init >----------------------------------------
// Purpose:    initializes the ranging module module
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID HCSR04Init (PHCSR04_CONFIG pConfig)
{
   // initialize each ranging module
   for (UI8 i = 0; i < HCSR04_COUNT; i++)
   {
      g_pModules[i].nTrigPin = pConfig[i].nTrigPin;
      g_pModules[i].nEchoPin = pConfig[i].nEchoPin;
      g_pModules[i].nReading = UI16_MAX;
      PinSetOutput(pConfig[i].nTrigPin);
      PinSetInput(pConfig[i].nEchoPin);
      // activate pin change interrupts for the echo pin
      if (pConfig[i].nEchoPin < 8)
      {
         PCICR  |= (1 << PCIE0);
         PCMSK0 |= (1 << (pConfig[i].nEchoPin % 8));
      }
      else if (pConfig[i].nEchoPin < 16)
      {
         PCICR  |= (1 << PCIE1);
         PCMSK1 |= (1 << (pConfig[i].nEchoPin % 8));
      }
      else
      {
         PCICR  |= (1 << PCIE2);
         PCMSK2 |= (1 << (pConfig[i].nEchoPin % 8));
      }
   }
   // configure the 16-bit timer
   TCCR1B |= (1<<CS11) | (1<<CS10);    // prescale = 64 (250kHz)
   TCCR1B |= (1<<WGM12);               // CTC mode, value at OCR1A
   TIMSK1 |= (1<<OCIE1A);              // enable compare interrupt A
   OCR1A  = F_CPU / 64 / 4 - 1;        // reset at 62499 ticks for 4Hz
}
//-----------< FUNCTION: HCSR04Read >----------------------------------------
// Purpose:    reads the ranging module value, in microseconds
// Parameters: nModule - the module to read
// Returns:    none
//---------------------------------------------------------------------------
UI16 HCSR04Read (UI8 nModule)
{
   return g_pModules[nModule].nReading;
}
//-----------< INTERRUPT: TIMER1_COMPA_vect >--------------------------------
// Purpose:    responds to 4Hz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER1_COMPA_vect)
{
   g_nModule = (g_nModule + 1) % HCSR04_COUNT;
   // set the trigger pin hi for a minimum of 10 microseconds to ping
   PinSetHi(g_pModules[g_nModule].nTrigPin);
   _delay_us(10);
   PinSetLo(g_pModules[g_nModule].nTrigPin);
}
//-----------< INTERRUPT: PCINT0_vect >--------------------------------------
// Purpose:    pin change event handler for bank 0
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
#ifdef HCSR04_PCINT0
ISR(PCINT0_vect)
{
   HandlePinChange();
}
#endif
//-----------< INTERRUPT: PCINT1_vect >--------------------------------------
// Purpose:    pin change event handler for bank 1
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
#ifdef HCSR04_PCINT1
ISR(PCINT1_vect)
{
   HandlePinChange();
}
#endif
//-----------< INTERRUPT: PCINT2_vect >--------------------------------------
// Purpose:    pin change event handler for bank 2
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
#ifdef HCSR04_PCINT2
ISR(PCINT2_vect)
{
   HandlePinChange();
}
#endif
