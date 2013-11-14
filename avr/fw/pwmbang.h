//===========================================================================
// Module:  pwmbang.h
// Purpose: PWM bit banger API
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
#ifndef __PWMBANG_H
#define __PWMBANG_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// PWM BANGER CONFIGURATION
// . PWMBANG_CHANNEL_COUNT:   number of output pins to bit-bang
// . PWMBANG_SERVO:           define to set servo defaults
// . PWMBANG_CYCLE_LEN:       PWM cycle length, in 0.1ms units
// . PWMBANG_DUTY_MIN:        PWM minimum duty cycle, in 0.1ms units
// . PWMBANG_DUTY_MAX:        PWM maximum duty cycle, in 0.1ms units
//===========================================================================
#ifndef PWMBANG_CHANNEL_COUNT
#  error Undefined channel count (PWMBANG_CHANNEL_COUNT)
#endif
#ifdef PWMBANG_SERVO
#  ifndef PWMBANG_CYCLE_LEN
#     define PWMBANG_CYCLE_LEN      200         // 20ms
#  endif
#  ifndef PWMBANG_DUTY_MIN
#     define PWMBANG_DUTY_MIN       10          // 1ms
#  endif
#  ifndef PWMBANG_DUTY_MAX
#     define PWMBANG_DUTY_MAX       20          // 2ms
#  endif
#else
#  ifndef PWMBANG_CYCLE_LEN
#     define PWMBANG_CYCLE_LEN      200         // 20ms
#  endif
#  ifndef PWMBANG_DUTY_MIN
#     define PWMBANG_DUTY_MIN       0           // 0ms
#  endif
#  ifndef PWMBANG_DUTY_MAX
#     define PWMBANG_DUTY_MAX       200         // 20ms
#  endif
#endif
#define PWMBANG_DUTY_RANGE          (PWMBANG_DUTY_MAX - PWMBANG_DUTY_MIN + 1)
//===========================================================================
// PWM BANGER DATA STRUCTURES
//===========================================================================
typedef struct tagPwmBangConfig
{
   UI8*  pPins;
} PWMBANG_CONFIG, *PPWMBANG_CONFIG;
//===========================================================================
// PWM BANGER API
//===========================================================================
// module initialization
VOID     PwmBangInit       (PWMBANG_CONFIG* pConfig);
// duty cycle control, in 0.1ms units
UI8      PwmBangGetDuty    (UI8 nChannel);
VOID     PwmBangSetDuty    (UI8 nChannel, UI8 nDuty);
// duty cycle control, in the range [0,1], mapped to min/max duty cycle
F32      PwmBangGetDutyF   (UI8 nChannel);
VOID     PwmBangSetDutyF   (UI8 nChannel, F32 nPoint);
#endif // __PWMBANG_H
