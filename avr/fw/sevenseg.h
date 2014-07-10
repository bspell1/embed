//===========================================================================
// Module:  sevenseg.h
// Purpose: seven-segment display array driver
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
#ifndef __SEVENSEG_H
#define __SEVENSEG_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// 7-SEGMENT DISPLAY CONFIGURATION
// . the display hardware consists of an array of 7-segment displays, a shift 
//   register, and a multiplexer
// . the 7-segment displays are wired with their corresponding anodes shorted
// . the shift register powers the anodes (1 per segment) of the display
// . the multiplexer selects the cathode to ground, based on the desired
//   digit in the array to light
// . in this configuration, the AVR can light an 8-digit display using only
//   6 pins: 3 for the shift register, and 3 for the multiplexer
// . SEVENSEG_DIGITS:         number of digits in the array (max = 8)
//===========================================================================
// configuration properties
#ifndef SEVENSEG_DIGITS
#  define SEVENSEG_DIGITS     (1)
#endif
// configuration structure
typedef struct tagSevenSegConfig
{
   UI8   nSROffset;           // shift register offset
   UI8*  pnMuxPins;           // multiplexer pin array: log2(SEVENSEG_DIGITS)
   BOOL  fReverse;            // reverse digit order?
} SEVENSEG_CONFIG, *PSEVENSEG_CONFIG;
//===========================================================================
// 7-SEGMENT SEGMENTS
// . bits for each of the seven digital segments plus the decimal point
//===========================================================================
#define SEVENSEG_TOP          0
#define SEVENSEG_TOPRIGHT     1
#define SEVENSEG_BOTTOMRIGHT  2
#define SEVENSEG_BOTTOM       3
#define SEVENSEG_BOTTOMLEFT   4
#define SEVENSEG_TOPLEFT      5
#define SEVENSEG_MIDDLE       6
#define SEVENSEG_POINT        7
//===========================================================================
// 7-SEGMENT DISPLAY API
//===========================================================================
// module initialization
VOID  SevenSegInit         (PSEVENSEG_CONFIG pConfig);
// array strobing
VOID  SevenSegStrobe       ();
// segment assignment
VOID  SevenSegSetSegment   (UI8 nDigit, UI8 nSegment, BOOL fValue);
// digit assignment
VOID  SevenSegClearDigit   (UI8 nDigit);
VOID  SevenSegSetDigit     (UI8 nDigit, UI8 nValue, BOOL fPoint);
VOID  SevenSegLShiftDigit  (UI8 nValue);
VOID  SevenSegRShiftDigit  (UI8 nValue);
// array assignment
VOID  SevenSegSet8         (I8 nValue);
VOID  SevenSegSet16        (I16 nValue);
VOID  SevenSegSet32        (I32 nValue);
VOID  SevenSegSetFP        (F32 nValue, UI8 nFractional);
// value assignment helpers
inline VOID SevenSegSetF (F32 nValue)
   { SevenSegSetFP(nValue, 1); }
#endif // __SEVENSEG_H  
