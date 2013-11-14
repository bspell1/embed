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
#include "pwmbang.h"
//-------------------[       Module Definitions        ]-------------------//
// channel numbers
#define ROTOR_FORE         0                    // forward rotor
#define ROTOR_AFT          1                    // aft rotor
#define ROTOR_PORT         2                    // port rotor
#define ROTOR_STAR         3                    // starboard rotor
//-------------------[        Module Variables         ]-------------------//
static UI8 g_nChannels[4]   = { UI8_MAX, };     // rotor channels on the servo banger
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SetThrust >-----------------------------------------
// Purpose:    updates the TLC5940 for a rotor channel
// Parameters: nRotor  - the rotor number to update (ROTOR_* above)
//             nThrust - rotor thrust [0,1]
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetThrust (UI8 nRotor, F32 nThrust)
{
   // clamp the thrust value and set the rotor duty cycle
   PwmBangSetDutyF(
      g_nChannels[nRotor], 
      Min(Max(nThrust, QUADROTOR_THRUST_MIN), QUADROTOR_THRUST_MAX)
   );
}
//-----------< FUNCTION: QuadRotorInit >-------------------------------------
// Purpose:    initializes the controller
// Parameters: pConfig - quadrotor configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadRotorInit (PQUADROTOR_CONFIG pConfig)
{
   g_nChannels[ROTOR_FORE] = pConfig->nForeChannel;
   g_nChannels[ROTOR_AFT]  = pConfig->nAftChannel;
   g_nChannels[ROTOR_PORT] = pConfig->nPortChannel;
   g_nChannels[ROTOR_STAR] = pConfig->nStarChannel;
   SetThrust(ROTOR_FORE, QUADROTOR_THRUST_MIN);
   SetThrust(ROTOR_AFT,  QUADROTOR_THRUST_MIN);
   SetThrust(ROTOR_PORT, QUADROTOR_THRUST_MIN);
   SetThrust(ROTOR_STAR, QUADROTOR_THRUST_MIN);
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
