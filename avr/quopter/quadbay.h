//===========================================================================
// Module:  quadbay.h
// Purpose: quadcopter bomb bay controller
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
#ifndef __QUADBAY_H
#define __QUADBAY_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// BOMB BAY CONTROLLER STRUCTURES
//===========================================================================
// configuration structure
typedef struct tagQuadBayConfig
{
} QUADBAY_CONFIG, *PQUADBAY_CONFIG;
//===========================================================================
// BOMB BAY CONTROLLER API
//===========================================================================
VOID  QuadBayInit    (PQUADBAY_CONFIG pConfig);
BOOL  QuadBayIsOpen  ();
VOID  QuadBayControl (BOOL bOpen);
// bomb bay controller helpers
inline BOOL QuadBayIsClosed ()
   { return !QuadBayIsOpen(); }
inline VOID QuadBayToggle ()
   { QuadBayControl(!QuadBayIsOpen()); }
#endif // __QUADBAY_H
