//===========================================================================
// Module:  quadmpu.h
// Purpose: quadcopter motion processor and sensor filter
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
#ifndef __QUADMPU_H
#define __QUADMPU_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// MPU SENSOR CONSTANTS
// . QUADMPU_SAMPLE_TIME      time between MPU samples, for integrating gyro speeds
//===========================================================================
#ifndef QUADMPU_SAMPLE_TIME
#  error QUADMPU_SAMPLE_TIME must be assigned
#endif
//===========================================================================
// MPU SENSOR STRUCTURES
//===========================================================================
typedef struct tagQuadMpuConfig
{
} QUADMPU_CONFIG, *PQUADMPU_CONFIG;
typedef struct tagQuadMpuSensor
{
   F32   nRollAngle;             // roll (X) angle in radians, from accel/gyro
   F32   nPitchAngle;            // pitch (Y) angle in radians, from accel/gyro
   F32   nYawRate;               // yaw (Z) rate in radians/sec
} QUADMPU_SENSOR, *PQUADMPU_SENSOR;
//===========================================================================
// MPU SENSOR API
//===========================================================================
VOID              QuadMpuInit       (PQUADMPU_CONFIG pConfig);
VOID              QuadMpuBeginRead  ();
QUADMPU_SENSOR*   QuadMpuEndRead    (PQUADMPU_SENSOR pSensor);
#endif // __QUADMPU_H
