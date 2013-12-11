//===========================================================================
// Module:  quadtel.h
// Purpose: quadcopter telemetrics transmitter
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
#ifndef __QUADTEL_H
#define __QUADTEL_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// TELEMETRICS STRUCTURES
//===========================================================================
// configuration structure
typedef struct tagQuadTelConfig
{
   PCSTR pszAddress;                // Quopter NRF24 transmit address
} QUADTEL_CONFIG, *PQUADTEL_CONFIG;
// input control structure
typedef struct tagQuadTelData
{
   I8    nRollAngle;
   I8    nPitchAngle;
   I8    nYawRate;
   UI8   nThrustInput;
   I8    nRollInput;
   I8    nPitchInput;
   I8    nYawInput;
   UI8   nCounter;
} QUADTEL_DATA, *PQUADTEL_DATA;
//===========================================================================
// TELEMETRICS API
//===========================================================================
VOID  QuadTelInit (PQUADTEL_CONFIG pConfig);
VOID  QuadTelSend (PQUADTEL_DATA pData);
#endif // __QUADTEL_H
