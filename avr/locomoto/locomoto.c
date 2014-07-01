//===========================================================================
// Module:  locomoto.c
// Purpose: LocoMoto motor driver program entry point
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
#include <math.h>
//-------------------[      Project Include Files      ]-------------------//
#include "locomoto.h"
#include "pid.h"
#include "spimast.h"
#include "shiftreg.h"
#include "nrf24.h"
#include "stepper.h"
#include "locopsx.h"
//-------------------[       Module Definitions        ]-------------------//
// locomoto configuration
#define THROTTLE_ABSMIN       ((F32)0.15f)   // desensitize PSX joystick
#define THROTTLE_ABSMAX       ((F32)1.0f)    // maximum absolute throttle
#define DELAY_MIN             15             // 1.5ms at 50 steps/r => 200 RPM
#define DELAY_MAX             50             // 5.0ms at 50 steps/r => 60 RPM
#define CONTROL_LOCKMIN       ((F32)0.1f)    // minimum left/right wheel differential
#define MOTOR_LEFT            0              // left motor shift register
#define MOTOR_RIGHT           1              // right motor shift register
#define CONTROL_PGAIN         0.02f          // PID controller P gain
#define CONTROL_IGAIN         0.0f           // PID controller I gain
#define CONTROL_DGAIN         0.0f           // PID controller D gain
// AVR pin configuration   
#define PIN_LED               PIN_D7
#define PIN_SHIFTREG_CLOCK    PIN_D2
#define PIN_SHIFTREG_LATCH    PIN_D3
#define PIN_SHIFTREG_DATA     PIN_D4
#define PIN_NRF24_SS          PIN_B1
#define PIN_NRF24_CE          PIN_B0
//-------------------[        Module Variables         ]-------------------//
static PID  g_LPid;
static PID  g_RPid;
static F32  g_nLThrottle = 0.0f;
static F32  g_nRThrottle = 0.0f;
//-------------------[        Module Prototypes        ]-------------------//
static VOID LocoMotoInit      ();
static VOID LocoMotoRun       ();
static VOID LocoMotoControl   (F32 nLControl, F32 nRControl);
static VOID LocoMotoRunMotor  (UI8 nMotor, F32 nThrottle);
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   LocoMotoInit();
   for ( ; ; )
      LocoMotoRun();
   return 0;
}
//-----------< FUNCTION: LocoMotoInit >--------------------------------------
// Purpose:    locomoto module initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoInit ()
{
   // protocol initialization
   sei();
   SpiInit();
   // hardware initialization
   PinSetOutput(PIN_LED);
   ShiftRegInit(
      &(SHIFTREG_CONFIG)
      {
         .nClockPin = PIN_SHIFTREG_CLOCK,
         .nLatchPin = PIN_SHIFTREG_LATCH,
         .nDataPin  = PIN_SHIFTREG_DATA
      }
   );
   Nrf24Init(
      &(NRF24_CONFIG)
      {
         .nSsPin = PIN_NRF24_SS,
         .nCePin = PIN_NRF24_CE
      }
   );
   Nrf24SetCrc(NRF24_CRC_16BIT);
   // module initialization
   StepMotorInit(
      (STEPMOTOR_CONFIG[STEPPER_COUNT])
      {
         { .nShiftReg = 0 },
         { .nShiftReg = 1 },
      }
   );
   LocoPsxInit(
      &(LOCOPSX_CONFIG)
      {
         .pszAddress = LOCOPSX_ADDRESS,
         .nPipe      = 1
      }
   );
   // controller initialization
   g_LPid.nControl = g_RPid.nControl = 0.0f;
   g_LPid.nPGain   = g_RPid.nPGain   = CONTROL_PGAIN;
   g_LPid.nIGain   = g_RPid.nIGain   = CONTROL_IGAIN;
   g_LPid.nDGain   = g_RPid.nDGain   = CONTROL_DGAIN;
   PidInit(&g_LPid);
   PidInit(&g_RPid);
}
//-----------< FUNCTION: LocoMotoRun >---------------------------------------
// Purpose:    locomoto main loop
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoRun ()
{
   static UI8 g_nCounter = 0;
   // read the current PSX pad state
   LOCOPSX_INPUT psx; memzero(&psx, sizeof(psx));
   if (LocoPsxEndRead(&psx) == NULL) 
      PinSetLo(PIN_LED);
   else if (g_nCounter == 0)
      PinToggle(PIN_LED);
   // update the throttle
   LocoMotoControl(psx.nLY, psx.nRY);
   // start the next PSX read
   LocoPsxBeginRead();
   g_nCounter++;
   _delay_ms(2);
}
//-----------< FUNCTION: LocoMotoControl >-----------------------------------
// Purpose:    controls the motor pair
// Parameters: nLControl - left throttle control
//             nRControl - right throttle control
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoControl (F32 nLControl, F32 nRControl)
{
   // lock the throttle values if they are close,
   // in order to stabilize straight steering
   if (fabs(nLControl - nRControl) < CONTROL_LOCKMIN)
      nLControl = nRControl = (nLControl + nRControl) / 2.0f;
   // update the controllers using the control values
   // . if throttling down, simply apply the control value
   // . otherwise, smooth the control input through the PID controller
   if (fabs(nLControl) < fabs(g_nLThrottle))
      g_nLThrottle = nLControl;
   else
   {
      PidUpdate(&g_LPid, nLControl, g_nLThrottle);
      g_nLThrottle += g_LPid.nControl;
   }
   if (fabs(nRControl) < fabs(g_nRThrottle))
      g_nRThrottle = nRControl;
   else
   {
      PidUpdate(&g_RPid, nRControl, g_nRThrottle);
      g_nRThrottle += g_RPid.nControl;
   }
   // set the throttle on each motor
   LocoMotoRunMotor(MOTOR_LEFT, -g_nLThrottle);
   LocoMotoRunMotor(MOTOR_RIGHT, g_nRThrottle);
}
//-----------< FUNCTION: LocoMotoRunMotor >----------------------------------
// Purpose:    runs a stepper motor
// Parameters: nMotor    - motor to run
//             nThrottle - motor speed
// Returns:    none
//---------------------------------------------------------------------------
VOID LocoMotoRunMotor (UI8 nMotor, F32 nThrottle)
{
   if (fabs(nThrottle) < THROTTLE_ABSMIN)
      StepMotorStop(nMotor);
   else
      StepMotorRun(
         nMotor, 
         Map(
            fabs(nThrottle), 
            THROTTLE_ABSMIN,
            THROTTLE_ABSMAX,
            DELAY_MAX, 
            DELAY_MIN
         ),
         nThrottle > 0 ? STEPMOTOR_FORWARD : STEPMOTOR_REVERSE
      );
}
