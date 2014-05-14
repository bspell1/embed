//===========================================================================
// Module:  stepper.c
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
#include "stepper.h"
#include "shiftreg.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
// motor state data
static volatile struct MOTOR
{
   UI8   nShiftReg;                       // shift register offset, in bytes
   UI8   nDelay;                          // stage delay, in 0.1ms units
   UI8   nTimer;                          // stage timer
   I16   nSteps;                          // number of steps (+/-) to run
   UI8   nStage;                          // current step stage (0-3)
} g_pMotors[STEPPER_COUNT];
// motor forward step stages
static const UI8 g_pStages[] = 
{
   0x99,                                  // 10011001b, 1010 stage
   0x96,                                  // 10010110b, 0110 stage
   0x66,                                  // 01100110b, 0101 stage
   0x69,                                  // 01101001b, 1001 stage
};
#define STAGE_COUNT (sizeof(g_pStages) / sizeof(*g_pStages))
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
   ShiftRegWrite8(g_pMotors[nMotor].nShiftReg, nData);
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
   for (UI8 nMotor = 0; nMotor < STEPPER_COUNT; nMotor++)
      g_pMotors[nMotor].nShiftReg = pConfig[nMotor].nShiftReg;
   // 8-bit clock 2, software, 10kHz
   RegSetHi(TCCR2A, WGM21);            // CTC mode, compare at OCR2A
   RegSetHi(TCCR2B, CS22);             // prescale = 64 (250kHz)
   OCR2A = F_CPU / 64 / 10000 - 1;     // reset OC2A at 25 ticks for 10kHz
}
//-----------< FUNCTION: StepMotorStop >-------------------------------------
// Purpose:    stops stepper motor processing for a given motor
// Parameters: nMotor - the stepper motor to stop
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorStop (UI8 nMotor)
{
   // clear the step count
   // the interrupt handler will gracefully shutdown
   g_pMotors[nMotor].nSteps = 0;
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
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      g_pMotors[nMotor].nDelay = Max(nDelay, 1);
      g_pMotors[nMotor].nSteps = nSteps;
      RegSetHi(TIMSK2, OCIE2B);
   }
}
//-----------< INTERRUPT: TIMER2_COMPB_vect >--------------------------------
// Purpose:    responds to 10kHz timer events
//             . the stepper delay determines the number of interrupts per 
//               motor that need to elapse before making a stage transition,
//               which determines the speed of the motor 
//               (lower delay = higher speed)
//             . each stage is represented by a pair of magnets to turn on 
//               (and a pair to turn off) to advance a quarter step
//             . each step consists of 4 stages; to run the motor in reverse
//               the step count should be less than zero; the number of
//               steps determines the distance traveled
//             . to run the motor continuously, the step count should be
//               either INT16_MIN or INT16_MAX
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPB_vect)
{
   BOOL bIdle = TRUE;
   for (UI8 nMotor = 0; nMotor < STEPPER_COUNT; nMotor++)
   {
      volatile struct MOTOR* pMotor = &g_pMotors[nMotor];
      // decrement the stage timer for the current motor
      // when it reaches zero, a stage transition should occur
      if (pMotor->nDelay != 0 && --pMotor->nTimer == 0)
      {
         // advance the motor to the next stage in the current step
         if (pMotor->nSteps > 0)
         {
            // moving forward, go to the next stage
            // decrement step count when cycle completes
            SetShiftRegister(nMotor, g_pStages[pMotor->nStage++]);
            if (pMotor->nStage == STAGE_COUNT)
            {
               pMotor->nStage = 0;
               if (pMotor->nSteps != I16_MAX)
                  pMotor->nSteps--;
            }
         }
         else if (pMotor->nSteps < 0)
         {
            // moving backward, go to the previous stage
            // increment step count when cycle completes
            SetShiftRegister(nMotor, g_pStages[STAGE_COUNT - 1 - pMotor->nStage++]);
            if (pMotor->nStage == STAGE_COUNT)
            {
               pMotor->nStage = 0;
               if (pMotor->nSteps != I16_MIN)
                  pMotor->nSteps++;
            }
         }
         else
         {
            // stopping, turn off all magnets to save power
            // and reset the delay to allow disabling the interrupt
            SetShiftRegister(nMotor, 0);
            pMotor->nDelay = pMotor->nStage = 0;
         }
         // reset the timer for the next stage
         pMotor->nTimer = pMotor->nDelay;
      }
      // if the delay hasn't yet been cleared, we are still running
      if (pMotor->nDelay != 0)
         bIdle = FALSE;
   }
   // if all motors are idle, disable the interrupt
   if (bIdle)
      RegSetLo(TIMSK2, OCIE2B);
}
