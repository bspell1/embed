//===========================================================================
// Module:  quadctrl.c
// Purpose: quadcopter rotor controller
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
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#include "quadctrl.h"
#include "tlc5940.h"
//-------------------[       Module Definitions        ]-------------------//
// channel numbers
#define ROTOR_FORE   0                                // forward rotor
#define ROTOR_AFT    1                                // aft rotor
#define ROTOR_PORT   2                                // port rotor
#define ROTOR_STAR   3                                // starboard rotor
// PWM ranges
// . PWM_MIN: minimum ESC duty cycle (1ms min forward)
// . PWM_MAX: maximum ESC duty cycle (1.5ms max forward, reverse is 1.5-2ms)
// . PWM_NEGMAX: maximum negative ESC duty cycle, for calibration
#define PWM_MIN      (F32)(1.0f * (4096.0f / 20.0f))
#define PWM_MAX      (F32)(1.5f * (4096.0f / 20.0f))
#define PWM_NEGMAX   (F32)(2.0f * (4096.0f / 20.0f))
#define PWM_RANGE    (PWM_MAX - PWM_MIN)
//-------------------[        Module Variables         ]-------------------//
static UI8 g_nTlc5940       = UI8_MAX;          // TLC5940 module number
static UI8 g_nChannels[4]   = { UI8_MAX, };     // rotor channels on the TLC5940
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SetDuty >-------------------------------------------
// Purpose:    updates the TLC5940 for a rotor channel
// Parameters: nRotor - the rotor number to update (ROTOR_* above)
//             nDuty  - TLC5940 duty cycle [0-4095]
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetDuty (UI8 nRotor, UI16 nDuty)
{
   // 12-bit TLC5940 duty cycle,
   // subtract from 4095 to invert since using pull-up resistor
   Tlc5940SetDuty(g_nTlc5940, g_nChannels[nRotor], 4095 - nDuty);
}
//-----------< FUNCTION: SetThrust >-----------------------------------------
// Purpose:    maps a thrust to duty cycle and updates a rotor channel
// Parameters: nRotor  - the rotor number to update (ROTOR_* above)
//             nThrust - rotor thrust [0,1]
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetThrust (UI8 nRotor, F32 nThrust)
{
   // clamp the thrust value and convert to duty cycle
   nThrust = Min(Max(nThrust, QUADROTOR_THRUST_MIN), QUADROTOR_THRUST_MAX);
   SetDuty(nRotor, (UI16)(nThrust * PWM_RANGE + PWM_MIN));
}
//-----------< FUNCTION: QuadRotorInit >-------------------------------------
// Purpose:    initializes the controller
// Parameters: pConfig - quadrotor configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadRotorInit (PQUADROTOR_CONFIG pConfig)
{
   g_nTlc5940              = pConfig->nTlc5940Module;
   g_nChannels[ROTOR_FORE] = pConfig->nForeChannel;
   g_nChannels[ROTOR_AFT]  = pConfig->nAftChannel;
   g_nChannels[ROTOR_PORT] = pConfig->nPortChannel;
   g_nChannels[ROTOR_STAR] = pConfig->nStarChannel;
   // calibrate the ESCs
   // . run each up to maximum (negative) duty cycle and hold
   // . run each down to minimum duty cycle and hold
   for (UI8 i = 0; i < 4; i++)
      SetDuty(i, (UI16)PWM_NEGMAX);
   _delay_ms(1000);
   for (UI8 i = 0; i < 4; i++)
      SetDuty(i, (UI16)PWM_MIN);
   _delay_ms(1000);
}
//-----------< FUNCTION: QuadRotorControl >----------------------------------
// Purpose:    sets the control signal for the quadrotors
// Parameters: nThrust - base thrust value [0-1]
//             nRoll   - roll value [-1,1]
//             nPitch  - pitch value [-1,1]
//             nYaw    - yaw value [-1,1]
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadRotorControl (F32 nThrust, F32 nRoll, F32 nPitch, F32 nYaw)
{
   F32 nFore = nThrust + nPitch / 2.0f + nYaw / 2.0f;
   F32 nAft  = nThrust - nPitch / 2.0f + nYaw / 2.0f;
   F32 nPort = nThrust - nRoll  / 2.0f - nYaw / 2.0f;
   F32 nStar = nThrust + nRoll  / 2.0f - nYaw / 2.0f;
   SetThrust(ROTOR_FORE, nFore);
   SetThrust(ROTOR_AFT,  nAft);
   SetThrust(ROTOR_PORT, nPort);
   SetThrust(ROTOR_STAR, nStar);
}
