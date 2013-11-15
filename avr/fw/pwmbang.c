//===========================================================================
// Module:  pwmbang.c
// Purpose: PWM bit banger API
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
#include "pwmbang.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
// current PWM channel states
static struct {
   UI8            nPin;             // output pin
   volatile UI16  nDuty;            // current duty cycle
} g_Channels[PWMBANG_CHANNEL_COUNT];
//-----------< FUNCTION: PwmBangInit >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID PwmBangInit (PWMBANG_CONFIG* pConfig)
{
   // initialize the PWM output pins
   memzero(g_Channels, sizeof(g_Channels));
   for (UI8 i = 0; i < PWMBANG_CHANNEL_COUNT; i++)
   {
      g_Channels[i].nPin = pConfig->pPins[i];
      PinSetLo(g_Channels[i].nPin);
      PinSetOutput(g_Channels[i].nPin);
   }
   // 8-bit timer 0, software, 100kHz
   RegSetHi(TCCR0A, WGM01);                  // CTC mode, compare at OCR0A
   RegSetHi(TCCR0B, CS00);                   // prescale = 64 (250kHz)
   RegSetHi(TCCR0B, CS01);                   // prescale = 64 (250kHz)
   RegSetHi(TIMSK0, OCIE0A);                 // enable compare interrupt A
   OCR0A = F_CPU / 64 / 1000 - 1;            // reset OC0A at 250 ticks for 100kHz
}
//-----------< FUNCTION: PwmBangGetDuty >------------------------------------
// Purpose:    gets the duty cycle for a PWM channel
// Parameters: nChannel - output channel number
// Returns:    the current duty cycle control value for the channel
//---------------------------------------------------------------------------
UI16 PwmBangGetDuty (UI8 nChannel)
{
   return g_Channels[nChannel].nDuty;
}
//-----------< FUNCTION: PwmBangSetDuty >------------------------------------
// Purpose:    sets the duty cycle for a PWM channel
// Parameters: nChannel - output channel number
//             nDuty    - duty cycle control 
//                        range: [PWMBANG_DUTY_MIN, PWMBANG_DUTY_MAX]
// Returns:    none
//---------------------------------------------------------------------------
VOID PwmBangSetDuty (UI8 nChannel, UI16 nDuty)
{
   g_Channels[nChannel].nDuty = 
      Min(Max(nDuty, PWMBANG_DUTY_MIN), PWMBANG_DUTY_MAX);
}
//-----------< FUNCTION: PwmBangGetDutyF >-----------------------------------
// Purpose:    gets the duty cycle for a PWM channel
// Parameters: nChannel - output channel number
// Returns:    the duty cycle for the channel in [0,1]
//---------------------------------------------------------------------------
F32 PwmBangGetDutyF (UI8 nChannel)
{
   return ((F32)PwmBangGetDuty(nChannel) - PWMBANG_DUTY_MIN) / PWMBANG_DUTY_RANGE;
}
//-----------< FUNCTION: PwmBangSetDutyF >-----------------------------------
// Purpose:    sets the duty cycle for a PWM channel
// Parameters: nChannel - output channel number
//             nPoint   - PWM duty cycle, in [0,1]
// Returns:    none
//---------------------------------------------------------------------------
VOID PwmBangSetDutyF (UI8 nChannel, F32 nPoint)
{
   PwmBangSetDuty(nChannel, (UI16)(nPoint * PWMBANG_DUTY_RANGE) + PWMBANG_DUTY_MIN);
}
//-----------< INTERRUPT: TIMER0_COMPA_vect >--------------------------------
// Purpose:    responds to 100kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER0_COMPA_vect)
{
   static volatile UI16 g_nTick = PWMBANG_CYCLE_LEN - 1;
   static volatile BOOL g_bDone = FALSE;
   if (++g_nTick == PWMBANG_CYCLE_LEN)
   {
      // begin the duty cycle for all channels
      g_nTick   = 0;
      g_bDone = FALSE;
      for (UI8 i = 0; i < PWMBANG_CHANNEL_COUNT; i++)
         PinSetHi(g_Channels[i].nPin);
   }
   else if (!g_bDone && g_nTick <= PWMBANG_DUTY_MAX)
   {
      // end the duty cycle for channels that have reached their length
      g_bDone = TRUE;
      for (UI8 i = 0; i < PWMBANG_CHANNEL_COUNT; i++)
         if (g_Channels[i].nDuty <= g_nTick)
            PinSetLo(g_Channels[i].nPin);
         else
            g_bDone = FALSE;
   }
}
