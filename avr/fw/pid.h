//===========================================================================
// Module:  pid.h
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
#ifndef __PID_H
#define __PID_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// PID STRUCTURES
//===========================================================================
// PID state
typedef struct tagPid
{
   F32   nPGain;              // proportional gain
   F32   nIGain;              // integral gain
   F32   nDGain;              // derivative gain
   F32   nControl;            // current PID control value
   F32   nISum;               // integral component sum
   F32   nDPrev;              // derivative component previous sensor value
} PID, *PPID;
//===========================================================================
// PID API
//===========================================================================
// PID initialization
VOID     PidInit              (PPID pPid);
// PID update
PPID     PidUpdate            (PPID pPid, F32 nInput, F32 nSensor);
#endif // __PID_H  
