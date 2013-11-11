//===========================================================================
// Module:  pid.c
// Purpose: PID (proportional, integral, derivative) controller
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
#include "pid.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: PidInit >-------------------------------------------
// Purpose:    initializes a PID controller state machine
// Parameters: pPid - PID state
// Returns:    none
//---------------------------------------------------------------------------
VOID PidInit (PPID pPid)
{
   pPid->nISum  = 0.0f;
   pPid->nDPrev = pPid->nControl;
}
//-----------< FUNCTION: PidUpdate >-----------------------------------------
// Purpose:    updates a PID state with an input and sensor value
// Parameters: pPid    - PID state
//             nInput  - input value
//             nSensor - sensor value
// Returns:    the updated PID state
//---------------------------------------------------------------------------
PPID PidUpdate (PPID pPid, F32 nInput, F32 nSensor)
{
   F32 nError = nInput - nSensor;
   F32 nISum  = pPid->nISum + nError;
   F32 nDVal  = nSensor - pPid->nDPrev;
   pPid->nControl = 
      pPid->nPGain * nError + 
      pPid->nIGain * nISum + 
      pPid->nDGain * nDVal;
   pPid->nISum  = nISum;
   pPid->nDPrev = nSensor;
   return pPid;
}
