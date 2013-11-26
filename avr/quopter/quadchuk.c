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
#define CHUK_JOYSTICK_RANGE      (CHUK_JOYSTICK_MAX - CHUK_JOYSTICK_MIN)
#define CHUK_JOYSTICK_ZERO       (CHUK_JOYSTICK_MIN + CHUK_JOYSTICK_RANGE / 2)
//-------------------[        Module Variables         ]-------------------//
// calibration readings, default to mean 
static UI8  g_zljx = CHUK_JOYSTICK_ZERO;
static UI8  g_zljy = CHUK_JOYSTICK_ZERO;
static UI8  g_zrjx = CHUK_JOYSTICK_ZERO;
static UI8  g_zrjy = CHUK_JOYSTICK_ZERO;
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: QuadChukInit >--------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadChukInit (PQUADCHUK_CONFIG pConfig)
{
   Nrf24SetPipeRXEnabled(pConfig->nPipe, TRUE);
   Nrf24SetRXAddress(pConfig->nPipe, pConfig->pszAddress);
   Nrf24SetPayloadLength(pConfig->nPipe, CHUK_PACKETSIZE);
   Nrf24SetPipeAutoAck(pConfig->nPipe, FALSE);
}
//-----------< FUNCTION: QuadChukBeginRead >---------------------------------
// Purpose:    starts an asynchronous chuk read operation
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadChukBeginRead ()
{
   Nrf24PowerOn(NRF24_MODE_RECV);
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
      BYTE pbPkt[CHUK_PACKETSIZE];
      Nrf24Recv(pbPkt, CHUK_PACKETSIZE);
      // decode the readings from the buffer
      UI8  ljx = pbPkt[0];                   // byte 0 is left joystick X
      UI8  ljy = pbPkt[1];                   // byte 1 is left joystick Y
      BOOL lbz = !((pbPkt[5] >> 0) & 0x1);   // byte 5[0] is !left z-button
      BOOL lbc = !((pbPkt[5] >> 1) & 0x1);   // byte 5[1] is !left c-button
      UI8  rjx = pbPkt[6];                   // byte 6 is right joystick X
      UI8  rjy = pbPkt[7];                   // byte 7 is right joystick Y
      BOOL rbz = !((pbPkt[11] >> 0) & 0x1);  // byte 11[0] is !right z-button
      BOOL rbc = !((pbPkt[11] >> 1) & 0x1);  // byte 11[1] is !right c-button
      // clamp the readings
      ljx = Clamp(ljx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      ljy = Clamp(ljy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      rjx = Clamp(rjx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      rjy = Clamp(rjy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      // if all buttons are pressed, reset calibration
      if (lbc && lbz && rbc && rbz)
      {
         g_zljx = ljx;
         g_zljy = ljy;
         g_zrjx = rjx;
         g_zrjy = rjy;
      }
      // calibrate and sign the readings
      I8  sljx =  (I8)(ljx - g_zljx);
      I8  sljy =  (I8)(ljy - g_zljy);
      I8  srjx =  (I8)(rjx - g_zrjx);
      I8  srjy =  (I8)(rjy - g_zrjy);
      // convert the readings to float and scale them
      pInput->nLeftJoystickX  = (F32)sljx / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nLeftJoystickY  = (F32)sljy / (CHUK_JOYSTICK_RANGE / 2);
      pInput->bLeftButtonZ    = lbz;
      pInput->bLeftButtonC    = lbc;
      pInput->nRightJoystickX = (F32)srjx / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nRightJoystickY = (F32)srjy / (CHUK_JOYSTICK_RANGE / 2);
      pInput->bRightButtonZ   = rbz;
      pInput->bRightButtonC   = rbc;
      return pInput;
   }
   return NULL;
}
