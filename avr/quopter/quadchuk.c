//===========================================================================
// Module:  quadchuk.c
// Purpose: quadcopter WiiChuk input receiver
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
#include "quadchuk.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
#define CHUK_PACKETSIZE          12
#define CHUK_JOYSTICK_MIN        30
#define CHUK_JOYSTICK_MAX        225
#define CHUK_JOYSTICK_ZERO       (CHUK_JOYSTICK_MIN + (CHUK_JOYSTICK_MAX - CHUK_JOYSTICK_MIN) / 2)
#define CHUK_ACCELERO_MIN        1
#define CHUK_ACCELERO_MAX        1023
#define CHUK_ACCELERO_ZERO       CHUK_ACCELERO_MIN + (CHUK_ACCELERO_MAX - CHUK_ACCELERO_MIN) / 2
//-------------------[        Module Variables         ]-------------------//
// calibration readings, default to mean 
static UI8  g_zljx = CHUK_JOYSTICK_ZERO;
static UI8  g_zljy = CHUK_JOYSTICK_ZERO;
static UI16 g_zlax = CHUK_ACCELERO_ZERO;
static UI16 g_zlay = CHUK_ACCELERO_ZERO;
static UI16 g_zlaz = CHUK_ACCELERO_ZERO;
static UI8  g_zrjx = CHUK_JOYSTICK_ZERO;
static UI8  g_zrjy = CHUK_JOYSTICK_ZERO;
static UI16 g_zrax = CHUK_ACCELERO_ZERO;
static UI16 g_zray = CHUK_ACCELERO_ZERO;
static UI16 g_zraz = CHUK_ACCELERO_ZERO;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: QuadChukInit >--------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadChukInit (PQUADCHUK_CONFIG pConfig)
{
   Nrf24SetRXAddress(pConfig->nPipe, pConfig->pszAddress);
   Nrf24SetPayloadLength(pConfig->nPipe, CHUK_PACKETSIZE);
}
//-----------< FUNCTION: QuadChukBeginRead >---------------------------------
// Purpose:    starts an asynchronous chuk read operation
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadChukBeginRead ()
{
   Nrf24PowerOn(NRF24_MODE_RECV);
   Nrf24BeginRecv(CHUK_PACKETSIZE);
}
//-----------< FUNCTION: QuadChukEndRead >-----------------------------------
// Purpose:    completes a chuk read operation
// Parameters: pInput - return the operation results via here
// Returns:    pInput if the input state was read
//             NULL otherwise
//---------------------------------------------------------------------------
PQUADCHUK_INPUT QuadChukEndRead (PQUADCHUK_INPUT pInput)
{
   // attempt to receive a chuk packet from the NRF24
   if (Nrf24ClearIrq(NRF24_IRQ_ALL) & NRF24_IRQ_RX_DR)
   {
      BYTE pbPacket[CHUK_PACKETSIZE];
      Nrf24EndRecv(pbPacket, CHUK_PACKETSIZE);
      // decode the readings from the buffer
      PBYTE  p = pbPacket;
      UI8  ljx = *p++;                       // byte 0 is left joystick X
      UI8  ljy = *p++;                       // byte 1 is left joystick Y
      UI16 lax = *p++ << 2;                  // byte 2 is left accel X upper 8 bits
      UI16 lay = *p++ << 2;                  // byte 3 is left accel Y upper 8 bits
      UI16 laz = *p++ << 2;                  // byte 4 is left accel Z upper 8 bits
      BOOL lbz = (*p >> 0) & 0x1;            // byte 5[0] is !left z-button
      BOOL lbc = (*p >> 1) & 0x1;            // byte 5[1] is !left c-button
      lax |= (*p >> 2) & 0x3;                // byte 5[2-3] is left accel X lower 2 bits
      lay |= (*p >> 4) & 0x3;                // byte 5[4-5] is left accel Y lower 2 bits
      laz |= (*p >> 6) & 0x3;                // byte 5[6-7] is left accel Z lower 2 bits
      p++;
      UI8  rjx = *p++;                       // byte 6 is right joystick X
      UI8  rjy = *p++;                       // byte 7 is right joystick Y
      UI16 rax = *p++ << 2;                  // byte 8 is right accel X upper 8 bits
      UI16 ray = *p++ << 2;                  // byte 9 is right accel Y upper 8 bits
      UI16 raz = *p++ << 2;                  // byte 10 is right accel Z upper 8 bits
      BOOL rbz = (*p >> 0) & 0x1;            // byte 11[0] is !right z-button
      BOOL rbc = (*p >> 1) & 0x1;            // byte 11[1] is !right c-button
      rax |= (*p >> 2) & 0x3;                // byte 11[2-3] is right accel X lower 2 bits
      ray |= (*p >> 4) & 0x3;                // byte 11[4-5] is right accel Y lower 2 bits
      raz |= (*p >> 6) & 0x3;                // byte 11[6-7] is right accel Z lower 2 bits
      p++;
      // if all buttons pressed, reset calibration
      // otherwise, calibrate the readings
      if (lbc && lbz && rbc && rbz)
      {
         g_zljx = ljx;
         g_zljy = ljy;
         g_zlax = lax;
         g_zlay = lay;
         g_zlaz = laz;
         g_zrjx = rjx;
         g_zrjy = rjy;
         g_zrax = rax;
         g_zray = ray;
         g_zraz = raz;
      }
      else
      {
         ljx -= g_zljx;
         ljy -= g_zljy;
         lax -= g_zlax;
         lay -= g_zlay;
         laz -= g_zlaz;
         rjx -= g_zrjx;
         rjy -= g_zrjy;
         rax -= g_zrax;
         ray -= g_zray;
         raz -= g_zraz;
      }
      // scale and convert the readings to float
      pInput->LeftJoystickX  = MapClamp(ljx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX, -1.0f, 1.0f);
      pInput->LeftJoystickY  = MapClamp(ljy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX, -1.0f, 1.0f);
      pInput->LeftAcceleroX  = MapClamp(lax, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->LeftAcceleroY  = MapClamp(lay, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->LeftAcceleroZ  = MapClamp(laz, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->LeftButtonZ    = lbz;
      pInput->LeftButtonC    = lbc;
      pInput->RightJoystickX = MapClamp(rjx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX, -1.0f, 1.0f);
      pInput->RightJoystickY = MapClamp(rjy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX, -1.0f, 1.0f);
      pInput->RightAcceleroX = MapClamp(rax, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->RightAcceleroY = MapClamp(ray, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->RightAcceleroZ = MapClamp(raz, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX, -1.0f, 1.0f);
      pInput->RightButtonZ   = rbz;
      pInput->RightButtonC   = rbc;
      return pInput;
   }
   return NULL;
}
