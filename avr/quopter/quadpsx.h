//===========================================================================
// Module:  quadpsx.h
// Purpose: quadcopter Playstation controller input receiver
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
#ifndef __QUADPSX_H
#define __QUADPSX_H
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
typedef struct tagQuadPsxConfig
{
   UI8   nPipe;                     // NRF24 receive pipe
   PCSTR pszAddress;                // PsxPad address
} QUADPSX_CONFIG, *PQUADPSX_CONFIG;
// input control structure
typedef struct tagQuadPsxInput
{
   BOOL  bSelect;                   // select button
   BOOL  bStart;                    // start button
   BOOL  bL1;                       // left button 1
   BOOL  bL2;                       // left button 2
   BOOL  bR1;                       // right button 1
   BOOL  bR2;                       // right button 2
   F32   nLX;                       // left X-axis joystick reading [-1,1]
   F32   nLY;                       // left Y-axis joystick reading [-1,1]
   F32   nRX;                       // right X-axis joystick reading [-1,1]
   F32   nRY;                       // right Y-axis joystick reading [-1,1]
} QUADPSX_INPUT, *PQUADPSX_INPUT;
//===========================================================================
// INPUT RECEIVER API
//===========================================================================
VOID              QuadPsxInit       (PQUADPSX_CONFIG pConfig);
VOID              QuadPsxBeginRead  ();
PQUADPSX_INPUT    QuadPsxEndRead    (PQUADPSX_INPUT pInput);
// receiver helpers
inline PQUADPSX_INPUT QuadPsxRead (PQUADPSX_INPUT pInput)
   { QuadPsxBeginRead(); return QuadPsxEndRead(pInput); }
#endif // __QUADPSX_H
