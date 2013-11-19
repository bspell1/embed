//===========================================================================
// Module:  quadrotr.h
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
#ifndef __QUADROTR_H
#define __QUADROTR_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// CONTROLLER CONSTANTS
// . QUADROTOR_THRUST_MIN        restricts the minimum thrust value
// . QUADROTOR_THRUST_MAX        restricts the maximum thrust value
// . QUADROTOR_PID_PGAIN         PID controller proportional gain
// . QUADROTOR_PID_IGAIN         PID controller integral gain
// . QUADROTOR_PID_DGAIN         PID controller differential gain
//===========================================================================
#ifndef QUADROTOR_THRUST_MIN
#  define QUADROTOR_THRUST_MIN   ((F32)0.0f)
#endif
#ifndef QUADROTOR_THRUST_MAX
#  define QUADROTOR_THRUST_MAX   ((F32)1.0f)
#endif
#define QUADROTOR_THRUST_RANGE   (QUADROTOR_THRUST_MAX - QUADROTOR_THRUST_MIN)
#ifndef QUADROTOR_PID_PGAIN
#  define QUADROTOR_PID_PGAIN    (0.5f)
#endif
#ifndef QUADROTOR_PID_IGAIN
#  define QUADROTOR_PID_IGAIN    (0.2f)
#endif
#ifndef QUADROTOR_PID_DGAIN
#  define QUADROTOR_PID_DGAIN    (0.1f)
#endif
//===========================================================================
// CONTROLLER STRUCTURES
//===========================================================================
// controller configuration
typedef struct tagQuadRotorConfig
{
   UI8   nTlc5940Module;      // TLC5940 module number
   UI8   nForeChannel;        // forward rotor channel on the TLC5940
   UI8   nAftChannel;         // aft rotor channel on the TLC5940
   UI8   nPortChannel;        // port-side rotor channel on the TLC5940
   UI8   nStarChannel;        // starboard-side rotor channel on the TLC5940
} QUADROTOR_CONFIG, *PQUADROTOR_CONFIG;
// controller input
typedef struct tagQuadRotorControl
{
   F32 nThrustInput;          // base thrust input control [0,1]
   F32 nRollInput;            // roll input control [-1,1]
   F32 nPitchInput;           // pitch input control [-1,1]
   F32 nYawInput;             // yaw input control [-1,1]
   F32 nRollSensor;           // roll sensor reading [-1,1]
   F32 nPitchSensor;          // pitch sensor reading [-1,1]
   F32 nYawSensor;            // yaw sensor reading [-1,1]
} QUADROTOR_CONTROL, *PQUADROTOR_CONTROL;
//===========================================================================
// CONTROLLER API
//===========================================================================
// controller initialization
VOID  QuadRotorInit     (PQUADROTOR_CONFIG pConfig);
// control operations
VOID  QuadRotorControl  (PQUADROTOR_CONTROL pControl);
#endif // __QUADROTR_H
