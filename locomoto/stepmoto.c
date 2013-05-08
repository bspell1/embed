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
#include <avr/interrupt.h>
//-------------------[      Project Include Files      ]-------------------//
#include "stepmoto.h"
#include "sx1509.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
// motor state data
static struct
{
   UI8   n1509Module;                              // SX1509 module number
   UI8   n1509Offset;                              // SX1509 starting pin (pink)
   UI8   nDelay;                                   // stage delay, in 0.1ms units
   UI8   nTimer;                                   // stage timer
   I8    nSteps;                                   // number of steps (+/-) to run
   UI8   nStage;                                   // current step stage (0-3)
} g_pMotors[STEPMOTO_COUNT];
// motor forward step stages
static UI8 g_pStages[] = 
{
   0x0A,                                           // 1010, pink/blue on
   0x06,                                           // 0110, orange/blue on
   0x05,                                           // 0101, orange/yellow on
   0x09                                            // 1001, pink/yellow
};
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SetDirReg >-----------------------------------------
// Purpose:    sets the direction register for a stepper motor
// Parameters: nMotor - motor to assign
//             nDir   - SX1509 pin direction to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetDirReg (UI8 nMotor, UI8 nDir)
{
   if (g_pMotors[nMotor].n1509Offset < 8)
   {
      UI8 nRegData = SX1509GetDirA(g_pMotors[nMotor].n1509Module);
      nRegData &= ~(0x0F << (g_pMotors[nMotor].n1509Offset % 8));
      nRegData |=  (nDir << (g_pMotors[nMotor].n1509Offset % 8));
      SX1509SetDirA(g_pMotors[nMotor].n1509Module, nRegData);
   }
   else
   {
      UI8 nRegData = SX1509GetDirB(g_pMotors[nMotor].n1509Module);
      nRegData &= ~(0x0F << (g_pMotors[nMotor].n1509Offset % 8));
      nRegData |=  (nDir << (g_pMotors[nMotor].n1509Offset % 8));
      SX1509SetDirB(g_pMotors[nMotor].n1509Module, nRegData);
   }
}
//-----------< FUNCTION: SetDataReg >----------------------------------------
// Purpose:    sets the data register for a stepper motor
// Parameters: nMotor - motor to assign
//             nData  - motor data to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetDataReg (UI8 nMotor, UI8 nData)
{
   if (g_pMotors[nMotor].n1509Offset < 8)
   {
      UI8 nRegData = SX1509GetDataA(g_pMotors[nMotor].n1509Module);
      nRegData &= ~(0x0F  << (g_pMotors[nMotor].n1509Offset % 8));
      nRegData |=  (nData << (g_pMotors[nMotor].n1509Offset % 8));
      SX1509SetDataA(g_pMotors[nMotor].n1509Module, nRegData);
   }
   else
   {
      UI8 nRegData = SX1509GetDataB(g_pMotors[nMotor].n1509Module);
      nRegData &= ~(0x0F  << (g_pMotors[nMotor].n1509Offset % 8));
      nRegData |=  (nData << (g_pMotors[nMotor].n1509Offset % 8));
      SX1509SetDataB(g_pMotors[nMotor].n1509Module, nRegData);
   }
}
//-----------< FUNCTION: StepMotorInit >-------------------------------------
// Purpose:    initializes the stepper motor module
// Parameters: pConfig - motor configuration array
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorInit (STEPMOTOR_CONFIG* pConfig)
{
   // initialize motor data
   memset(g_pMotors, 0, sizeof(g_pMotors));
   for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
   {
      // configure module/pin offsets
      g_pMotors[nMotor].n1509Module = pConfig[nMotor].n1509Module;
      g_pMotors[nMotor].n1509Offset = pConfig[nMotor].n1509Offset;
      // enable output on motor pins
      SetDataReg(nMotor, 0);
      SetDirReg(nMotor, 0);
   }
   // 8-bit clock 2, software, 10kHz
   RegSetHi(TCCR2A, WGM21);                            // CTC mode, compare at OCR2A
   RegSetHi(TCCR2B, CS22);                             // prescale = 64 (250kHz)
   OCR2A = F_CPU / 64 / 10000 - 1;                       // reset OC2A at 25 ticks for 10kHz
   RegSetHi(TIMSK2, OCIE2B);                           // TODO: remove
}
//-----------< FUNCTION: StepMotorIsBusy >-----------------------------------
// Purpose:    polls the stepper motor's busy status
// Parameters: none
// Returns:    true if the stepper motor is running
//             false otherwise
//---------------------------------------------------------------------------
BOOL StepMotorIsBusy ()
{
   return RegGet(TIMSK1, OCIE1B);
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
// Returns:    none
//---------------------------------------------------------------------------
VOID StepMotorRun (UI8 nMotor, UI8 nDelay, I8 nSteps)
{
   StepMotorStop(nMotor);
   if (nSteps != 0)
   {
      g_pMotors[nMotor].nDelay = nDelay;
      g_pMotors[nMotor].nTimer = 0;
      g_pMotors[nMotor].nSteps = nSteps;
      RegSetHi(TIMSK1, OCIE1B);
   }
}
//-----------< INTERRUPT: TIMER2_COMPB_vect >--------------------------------
// Purpose:    responds to 10kHz timer events
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(TIMER2_COMPB_vect)
{
   // uninterruptible phase, count down time to next stage for each motor
   for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
      if (g_pMotors[nMotor].nSteps != 0 && g_pMotors[nMotor].nTimer != 0)
         g_pMotors[nMotor].nTimer--;
   // interruptible phase, update motor registers
   static UI8 nLock = 0;
   if (nLock++ == 0)
   {
      sei();
      for (UI8 nMotor = 0; nMotor < STEPMOTO_COUNT; nMotor++)
      {
         if (g_pMotors[nMotor].nTimer == 0)
         {
            if (g_pMotors[nMotor].nSteps > 0)
            {
               // moving forward, go to the next stage
               // decrement step count when cycle completes
               UI8 nBankData = g_pStages[g_pMotors[nMotor].nStage++];
               if (g_pMotors[nMotor].nStage == 4)
               {
                  g_pMotors[nMotor].nStage = 0;
                  g_pMotors[nMotor].nSteps--;
               }
               SetDataReg(nMotor, nBankData);
            }
            else if (g_pMotors[nMotor].nSteps < 0)
            {
               // moving backward, go to the previous stage
               // increment step count when cycle completes
               UI8 nBankData = g_pStages[3 - g_pMotors[nMotor].nStage++];
               if (g_pMotors[nMotor].nStage == 4)
               {
                  g_pMotors[nMotor].nStage = 0;
                  g_pMotors[nMotor].nSteps++;
               }
               SetDataReg(nMotor, nBankData);
            }
            if (g_pMotors[nMotor].nSteps != 0)
               g_pMotors[nMotor].nTimer = g_pMotors[nMotor].nDelay;
         }
      }
   }
   nLock--;
   /* TODO: disable the ISR if no more steps are needed
   if (bDone)
      RegSetLo(TIMSK1, OCIE1B);
   */
}
