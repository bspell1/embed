//===========================================================================
// Module:  quadchuk.h
// Purpose: quadcopter WiiChuk input receiver
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
#ifndef __QUADCHUK_H
#define __QUADCHUK_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __QUOPTER_H
#include "quopter.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// INPUT RECEIVER STRUCTURES
//===========================================================================
// configuration structure
typedef struct tagQuadChukConfig
{
   UI8   nPipe;                     // NRF24 receive pipe
   PCSTR pszAddress;                // WiiChuk address
} QUADCHUK_CONFIG, *PQUADCHUK_CONFIG;
// input control structure
typedef struct tagQuadChukInput
{
   F32   LeftJoystickX;             // left chuk X-axis joystick reading [-1,1]
   F32   LeftJoystickY;             // left chuk Y-axis joystick reading [-1,1]
   F32   LeftAcceleroX;             // left chuk X-axis accelerometer reading [-1,1]
   F32   LeftAcceleroY;             // left chuk Y-axis accelerometer reading [-1,1]
   F32   LeftAcceleroZ;             // left chuk Z-axis accelerometer reading [-1,1]
   BOOL  LeftButtonZ;               // left chuk Z button
   BOOL  LeftButtonC;               // left chuk C button
   F32   RightJoystickX;            // right chuk X-axis joystick reading [-1,1]
   F32   RightJoystickY;            // right chuk Y-axis joystick reading [-1,1]
   F32   RightAcceleroX;            // right chuk X-axis accelerometer reading [-1,1]
   F32   RightAcceleroY;            // right chuk Y-axis accelerometer reading [-1,1]
   F32   RightAcceleroZ;            // right chuk Z-axis accelerometer reading [-1,1]
   BOOL  RightButtonZ;              // right chuk Z button
   BOOL  RightButtonC;              // right chuk C button
} QUADCHUK_INPUT, *PQUADCHUK_INPUT;
//===========================================================================
// INPUT RECEIVER API
//===========================================================================
VOID              QuadChukInit      (PQUADCHUK_CONFIG pConfig);
VOID              QuadChukBeginRead ();
PQUADCHUK_INPUT   QuadChukEndRead   (PQUADCHUK_INPUT pInput);
// receiver helpers
inline PQUADCHUK_INPUT QuadChukRead (PQUADCHUK_INPUT pInput)
   { QuadChukBeginRead(); return QuadChukEndRead(pInput); }
#endif // __QUADCHUK_H
