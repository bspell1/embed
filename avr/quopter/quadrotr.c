//===========================================================================
// Module:  quadrotr.c
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
#include "quadrotr.h"
#include "tlc5940.h"
#include "pid.h"
//-------------------[       Module Definitions        ]-------------------//
// channel numbers
#define ROTOR_BOW    0                                // forward rotor
#define ROTOR_STERN  1                                // aft rotor
#define ROTOR_PORT   2                                // port rotor
#define ROTOR_STAR   3                                // starboard rotor
// PID modules
#define PID_ROLL     0
#define PID_PITCH    1
#define PID_YAW      2
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
static PID g_pid[3];                            // PID controllers
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
   // configure the TLC5940 channels
   g_nTlc5940               = pConfig->nTlc5940Module;
   g_nChannels[ROTOR_BOW]   = pConfig->nBowChannel;
   g_nChannels[ROTOR_STERN] = pConfig->nSternChannel;
   g_nChannels[ROTOR_PORT]  = pConfig->nPortChannel;
   g_nChannels[ROTOR_STAR]  = pConfig->nStarChannel;
   // initialize the PID controllers
   for (UI8 i = 0; i < sizeof(g_pid) / sizeof(*g_pid); i++)
   {
      g_pid[i].nPGain   = QUADROTOR_PID_PGAIN;
      g_pid[i].nIGain   = QUADROTOR_PID_IGAIN;
      g_pid[i].nDGain   = QUADROTOR_PID_DGAIN;
      g_pid[i].nControl = 0.0f;
      PidInit(&g_pid[i]);
   }
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
// Parameters: control input and sensors
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadRotorControl (PQUADROTOR_CONTROL pControl)
{
   // pass the input+sensor readings through the PID controllers
   PidUpdate(&g_pid[PID_ROLL], pControl->nRollInput, pControl->nRollSensor);
   PidUpdate(&g_pid[PID_PITCH], pControl->nPitchInput, pControl->nPitchSensor);
   PidUpdate(&g_pid[PID_YAW], pControl->nYawInput, pControl->nYawSensor);
   F32 nThrust = pControl->nThrustInput;
   F32 nRoll   = g_pid[PID_ROLL].nControl;
   F32 nPitch  = g_pid[PID_PITCH].nControl;
   F32 nYaw    = g_pid[PID_YAW].nControl;
   // convert the control values to individual rotor thrusts
   F32 nBow   = nThrust + nPitch / 2.0f + nYaw / 2.0f;
   F32 nStern = nThrust - nPitch / 2.0f + nYaw / 2.0f;
   F32 nPort  = nThrust - nRoll  / 2.0f - nYaw / 2.0f;
   F32 nStar  = nThrust + nRoll  / 2.0f - nYaw / 2.0f;
   // send the thrust signals to the ESCs through the TLC5940
   SetThrust(ROTOR_BOW,   nBow);
   SetThrust(ROTOR_STERN, nStern);
   SetThrust(ROTOR_PORT,  nPort);
   SetThrust(ROTOR_STAR,  nStar);
}
