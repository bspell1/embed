//===========================================================================
// Module:  stepmoto.c
// Purpose: stepper motor driver module
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
#include "stepmoto.h"
#include "shiftreg.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
// motor state data
static volatile struct
{
   UI8   nSRNibble;                       // shift register offset, in nibbles
   UI8   nDelay;                          // stage delay, in 0.1ms units
   UI8   nTimer;                          // stage timer
   I16   nSteps;                          // number of steps (+/-) to run
   UI8   nStage;                          // current step stage (0-3)
} g_pMotors[STEPMOTO_COUNT];
// motor forward step stages
static const UI8 g_pStages[] = 
{
   0xA,                                   // 1010, pink/blue on
   0x6,                                   // 0110, orange/blue on
   0x5,                                   // 0101, orange/yellow on
   0x9                                    // 1001, pink/yellow on
};
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SetShiftRegister >----------------------------------
// Purpose:    sets the shift register for a stepper motor
// Parameters: nMotor - motor to assign
//             nData  - motor data to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetShiftRegister (UI8 nMotor, UI8 nData)
{
   ShiftRegWrite4(g_pMotors[nMotor].nSRNibble, nData);
}
//-----------< FUNCTION: StepMotorInit >-------------------------------------
// Purpose:    initializes the stepper motor module
// Parameters: pConfig - motor configuration array
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorInit (STEPMOTOR_CONFIG* pConfig)
{
   // initialize motor data
   memzero((PVOID)g_pMotors, sizeof(g_pMotors));
   for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
      g_pMotors[nMotor].nSRNibble = pConfig[nMotor].nSRNibble;
   // 8-bit clock 2, software, 10kHz
   RegSetHi(TCCR2A, WGM21);                              // CTC mode, compare at OCR2A
   RegSetHi(TCCR2B, CS22);                               // prescale = 64 (250kHz)
   OCR2A = F_CPU / 64 / 10000 - 1;                       // reset OC2A at 25 ticks for 10kHz
}
//-----------< FUNCTION: StepMotorIsBusy >-----------------------------------
// Purpose:    polls the stepper motor's busy status
// Parameters: none
// Returns:    true if the stepper motor is running
//             false otherwise
//---------------------------------------------------------------------------
BOOL StepMotorIsBusy ()
{
   return RegGet(TIMSK2, OCIE2B);
}
//-----------< FUNCTION: StepMotorWait >-------------------------------------
// Purpose:    waits until the stepper motor stops running
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorWait ()
{
   while (StepMotorIsBusy())
      ;
}
//-----------< FUNCTION: StepMotorStop >-------------------------------------
// Purpose:    stops stepper motor processing for a given motor
// Parameters: nMotor - the stepper motor to stop
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorStop (UI8 nMotor)
{
   // reset motor state and turn off all pins
   g_pMotors[nMotor].nSteps = 0;
   g_pMotors[nMotor].nDelay = 0;
   g_pMotors[nMotor].nTimer = 0;
   g_pMotors[nMotor].nStage = 0;
}
//-----------< FUNCTION: StepMotorStopAll >----------------------------------
// Purpose:    stops all stepper motor processing immediately
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorStopAll  ()
{
   for (UI8 i = 0; i < STEPMOTO_COUNT; i++)
      StepMotorStop(i);
}
//-----------< FUNCTION: StepMotorRun >--------------------------------------
// Purpose:    starts the stepper motor
// Parameters: nMotor - the stepper motor to run
//             nDelay - the stepper stage delay, in 0.1ms units
//             nSteps - the number of steps to run
//                      > 0     => run forward |nSteps| steps
//                      < 0     => run reverse |nSteps| steps
//                      I16_MAX => run forward continuously
//                      I16_MIN => run reverse continuously
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorRun (UI8 nMotor, UI8 nDelay, I16 nSteps)
{
   StepMotorStop(nMotor);
   if (nSteps != 0)
   {
      g_pMotors[nMotor].nDelay = nDelay;
      g_pMotors[nMotor].nTimer = 0;
      g_pMotors[nMotor].nSteps = nSteps;
      RegSetHi(TIMSK2, OCIE2B);
   }
}
//-----------< INTERRUPT: TIMER2_COMPB_vect >--------------------------------
// Purpose:    responds to 10kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPB_vect)
{
   // uninterruptible phase
   // count down time to next stage for each motor
   for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
      if (g_pMotors[nMotor].nSteps != 0 && g_pMotors[nMotor].nTimer != 0)
         g_pMotors[nMotor].nTimer--;
   static volatile UI8 g_nLock = 0;
   if (g_nLock++ == 0)
   {
      // interruptible phase
      // update motor registers
      sei();
      for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
      {
         if (g_pMotors[nMotor].nTimer == 0)
         {
            if (g_pMotors[nMotor].nSteps > 0)
            {
               // moving forward, go to the next stage
               // decrement step count when cycle completes
               UI8 nRegData = g_pStages[g_pMotors[nMotor].nStage++];
               if (g_pMotors[nMotor].nStage == 4)
               {
                  g_pMotors[nMotor].nStage = 0;
                  if (g_pMotors[nMotor].nSteps != I16_MAX)
                     g_pMotors[nMotor].nSteps--;
               }
               SetShiftRegister(nMotor, nRegData);
            }
            else if (g_pMotors[nMotor].nSteps < 0)
            {
               // moving backward, go to the previous stage
               // increment step count when cycle completes
               UI8 nRegData = g_pStages[3 - g_pMotors[nMotor].nStage++];
               if (g_pMotors[nMotor].nStage == 4)
               {
                  g_pMotors[nMotor].nStage = 0;
                  if (g_pMotors[nMotor].nSteps != I16_MIN)
                     g_pMotors[nMotor].nSteps++;
               }
               SetShiftRegister(nMotor, nRegData);
            }
            if (g_pMotors[nMotor].nSteps != 0)
               g_pMotors[nMotor].nTimer = g_pMotors[nMotor].nDelay;
         }
      }
      // uninterruptible phase
      // determine whether all motors are idle, disable interrupts if so
      cli();
      BOOL bIdle = TRUE;
      for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
         if (g_pMotors[nMotor].nSteps != 0)
            bIdle = FALSE;
      if (bIdle)
         RegSetLo(TIMSK2, OCIE2B);
   }
   g_nLock--;
}
