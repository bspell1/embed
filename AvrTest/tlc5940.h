//===========================================================================
// Module:  tlc5940.h
// Purpose: TLC4950 PWM driver
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
#ifndef __AVRTEST_H
#include "avrtest.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
VOID     Tlc5940Init       ();
UI16     Tlc5940GetDuty    (UI8 nChannel);
VOID     Tlc5940SetDuty    (UI8 nChannel, UI16 nDuty);
#endif // __TLC5940_H
