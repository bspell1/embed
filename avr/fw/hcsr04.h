//===========================================================================
// Module:  hcsr04.h
// Purpose: HCSR04 ultrasonic ranging module driver
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
#ifndef __HCSR04_H
#define __HCSR04_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// RANGING MODULE CONFIGURATION
// . HCSR04_COUNT:            number of components attached
// . HCSR04_PCINT0:           enable the interrupt vector for PCINT0
// . HCSR04_PCINT1:           enable the interrupt vector for PCINT0
// . HCSR04_PCINT2:           enable the interrupt vector for PCINT0
//===========================================================================
// configuration properties
#ifndef HCSR04_COUNT
#  define HCSR04_COUNT        (1)
#endif
#if !defined(HCSR04_PCINT0) && !defined(HCSR04_PCINT1) && !defined(HCSR04_PCINT2)
#  warning No pin change interrupt enabled for HCSR04, no readings will be taken
#endif
// configuration structure
typedef struct tagHCSR04Config
{
   UI8   nTrigPin;            // trigger (output) pin
   UI8   nEchoPin;            // echo (input) pin
} HCSR04_CONFIG, *PHCSR04_CONFIG;
//===========================================================================
// RANGING MODULE INTERFACE
//===========================================================================
// ranging  module API
VOID  HCSR04Init     (PHCSR04_CONFIG pConfig);
UI16  HCSR04Read     (UI8 nModule);
#endif // __HCSR04_H
