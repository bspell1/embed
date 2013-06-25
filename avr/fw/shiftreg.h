//===========================================================================
// Module:  shiftreg.h
// Purpose: 74HC595 shift register driver
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
#ifndef __SHIFTREG_H
#define __SHIFTREG_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// SHIFT REGISTER CONFIGURATION
// . SHIFTREG_SIZE:           size of the shift register array, in bytes
//===========================================================================
// configuration properties
#ifndef SHIFTREG_SIZE
#  define SHIFTREG_SIZE       (1)
#endif
// configuration structure
typedef struct tagShiftRegConfig
{
   UI8   nClockPin;                 // shift register clock output (SHCP)
   UI8   nLatchPin;                 // storage register clock output (STCP)
   UI8   nDataPin;                  // serial data output (DS)
} SHIFTREG_CONFIG, *PSHIFTREG_CONFIG;
//===========================================================================
// SHIFT REGISTER INTERFACE
//===========================================================================
// shift register API
VOID  ShiftRegInit   (PSHIFTREG_CONFIG pConfig);
VOID  ShiftRegRead   (PBYTE pbRegister);
VOID  ShiftRegWrite  (PCBYTE pbRegister);
BIT   ShiftRegRead1  (UI8 nOffset);
VOID  ShiftRegWrite1 (UI8 nOffset, BIT bValue);
UI8   ShiftRegRead4  (UI8 nOffset);
VOID  ShiftRegWrite4 (UI8 nOffset, UI8 nValue);
UI8   ShiftRegRead8  (UI8 nOffset);
VOID  ShiftRegWrite8 (UI8 nOffset, UI8 nValue);
#endif // __SHIFTREG_H
