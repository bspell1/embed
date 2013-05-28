//===========================================================================
// Module:  stepmoto.h
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
#ifndef __STEPMOTO_H
#define __STEPMOTO_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// STEPPER MOTOR CONFIGURATION
// . STEPMOTO_COUNT: the number of stepper motors in the circuit
//===========================================================================
#ifndef STEPMOTO_COUNT
#  define STEPMOTO_COUNT      2
#endif
//===========================================================================
// STEPPER MOTOR STRUCTURES
//===========================================================================
typedef struct tagStepMotorConfig
{
   UI8   n1509Module;                              // SX1509 module numbers
   UI8   n1509Offset;                              // SX1509 starting pins
} STEPMOTOR_CONFIG, *PSTEPMOTOR_CONFIG;
//===========================================================================
// STEPPER MOTOR PARAMETERS
#define STEPMOTOR_STEP_MIN    (I16_MIN + 1)
#define STEPMOTOR_STEP_MAX    (I16_MAX - 1)
#define STEPMOTOR_FORWARD     (I16_MAX)
#define STEPMOTOR_REVERSE     (I16_MIN)
//===========================================================================
//===========================================================================
// STEPPER MOTOR API
//===========================================================================
VOID     StepMotorInit        (STEPMOTOR_CONFIG* pConfig);
BOOL     StepMotorIsBusy      ();
VOID     StepMotorWait        ();
VOID     StepMotorStop        (UI8 nMotor);
VOID     StepMotorStopAll     ();
VOID     StepMotorRun         (UI8 nMotor, UI8 nDelay, I16 nSteps);
// motor helpers
inline VOID StepMotorRunForward (UI8 nMotor, UI8 nDelay)
   { StepMotorRun(nMotor, nDelay, STEPMOTOR_FORWARD); }
inline VOID StepMotorRunReverse (UI8 nMotor, UI8 nDelay)
   { StepMotorRun(nMotor, nDelay, STEPMOTOR_REVERSE); }
#endif // __STEPMOTO_H
