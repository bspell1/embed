//===========================================================================
// Module:  quadctrl.h
// Purpose: quadcopter rotor controller
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
#ifndef __QUADCTRL_H
#define __QUADCTRL_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// CONTROLLER CONSTANTS
//===========================================================================
// min/max rotor thrust values, inclusive
#ifndef QUADROTOR_THRUST_MIN
#  define QUADROTOR_THRUST_MIN   ((F32)0.0f)
#endif
#ifndef QUADROTOR_THRUST_MAX
#  define QUADROTOR_THRUST_MAX   ((F32)1.0f)
#endif
#define QUADROTOR_THRUST_RANGE   (QUADROTOR_THRUST_MAX - QUADROTOR_THRUST_MIN)
//===========================================================================
// CONTROLLER STRUCTURES
//===========================================================================
// controller configuration
typedef struct tagQuadRotorConfig
{
   UI8   nForeChannel;        // forward rotor channel on the servo banger
   UI8   nAftChannel;         // aft rotor channel on the servo banger
   UI8   nPortChannel;        // port-side rotor channel on the servo banger
   UI8   nStarChannel;        // starboard-side rotor channel on the servo banger
} QUADROTOR_CONFIG, *PQUADROTOR_CONFIG;
//===========================================================================
// CONTROLLER API
//===========================================================================
// controller initialization
VOID  QuadRotorInit     (PQUADROTOR_CONFIG pConfig);
// control operations
VOID  QuadRotorControl  (F32 nThrust, F32 nRoll, F32 nPitch, F32 nYaw);
#endif // __QUADCTRL_H
