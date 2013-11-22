//===========================================================================
// Module:  tlc5940.h
// Purpose: TLC5940 PWM driver
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
#ifndef __TLC5940_H
#define __TLC5940_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// TLC5940 CONFIGURATION
// . TLC5940_COUNT:     the number of 5940 modules in the daisy chain
// . TLC5940_FREQ:      PWM frequency, in Hz
// . TLC5940_BLSCALE:   clock prescaler for the blanking clock
// . TLC5940_BLTICK:    CTC match for the blanking clock, in cpu/prescaler/freq/CTC units
// . TLC5940_SERVO:     assigns servo defaults (50Hz cycle)
//===========================================================================
#if F_CPU != 16000000
#  error TLC5940 currently supports only 16MHz CPU speeds
#endif
#ifndef TLC5940_COUNT
#  define TLC5940_COUNT          1
#endif
#ifdef TLC5940_SERVO
#  define TLC5940_FREQ           50       // 50 Hz PWM cycle
#  define TLC5940_BLSCALE        128      // prescale at 128 for 125kHz
#  define TLC5940_BLTICK         20       // blank every 20 ticks for 50Hz
#endif
//===========================================================================
// TLC5940 DATA STRUCTURES
//===========================================================================
typedef struct tagTlc5940_Config
{
   UI8   nPinBlank;                       // TLC5940 23 (BLANK)
   UI8   nPinSClk;                        // TLC5940 25 (SCLK)
   UI8   nPinSIn;                         // TLC5940 26 (SIN)
   UI8   nPinXlat;                        // TLC5940 24 (XLAT)
   UI8   nPinGSClk;                       // TLC5940 18 (GSCLK)
} TLC5940_CONFIG, *PTLC5940_CONFIG;
//===========================================================================
// TLC5940 API
//===========================================================================
VOID     Tlc5940Init       (TLC5940_CONFIG* pConfig);
UI16     Tlc5940GetDuty    (UI8 nModule, UI8 nChannel);
VOID     Tlc5940SetDuty    (UI8 nModule, UI8 nChannel, UI16 nDuty);
#endif // __TLC5940_H
