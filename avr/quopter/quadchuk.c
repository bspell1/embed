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
#define CHUK_ACCELERO_MIN        1
#define CHUK_ACCELERO_MAX        1023
#define CHUK_ACCELERO_RANGE      (CHUK_ACCELERO_MAX - CHUK_ACCELERO_MIN)
#define CHUK_ACCELERO_ZERO       (CHUK_ACCELERO_MIN + CHUK_ACCELERO_RANGE / 2)
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
      UI16 lax = pbPkt[2] << 2;              // byte 2 is left accel X upper 8 bits
      UI16 lay = pbPkt[3] << 2;              // byte 3 is left accel Y upper 8 bits
      UI16 laz = pbPkt[4] << 2;              // byte 4 is left accel Z upper 8 bits
      BOOL lbz = !((pbPkt[5] >> 0) & 0x1);   // byte 5[0] is !left z-button
      BOOL lbc = !((pbPkt[5] >> 1) & 0x1);   // byte 5[1] is !left c-button
      lax |= (pbPkt[5] >> 2) & 0x3;          // byte 5[2-3] is left accel X lower 2 bits
      lay |= (pbPkt[5] >> 4) & 0x3;          // byte 5[4-5] is left accel Y lower 2 bits
      laz |= (pbPkt[5] >> 6) & 0x3;          // byte 5[6-7] is left accel Z lower 2 bits
      UI8  rjx = pbPkt[6];                   // byte 6 is right joystick X
      UI8  rjy = pbPkt[7];                   // byte 7 is right joystick Y
      UI16 rax = pbPkt[8] << 2;              // byte 8 is right accel X upper 8 bits
      UI16 ray = pbPkt[9] << 2;              // byte 9 is right accel Y upper 8 bits
      UI16 raz = pbPkt[10] << 2;             // byte 10 is right accel Z upper 8 bits
      BOOL rbz = !((pbPkt[11] >> 0) & 0x1);  // byte 11[0] is !right z-button
      BOOL rbc = !((pbPkt[11] >> 1) & 0x1);  // byte 11[1] is !right c-button
      rax |= (pbPkt[11] >> 2) & 0x3;         // byte 11[2-3] is right accel X lower 2 bits
      ray |= (pbPkt[11] >> 4) & 0x3;         // byte 11[4-5] is right accel Y lower 2 bits
      raz |= (pbPkt[11] >> 6) & 0x3;         // byte 11[6-7] is right accel Z lower 2 bits
      // clamp the readings
      ljx = Clamp(ljx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      ljy = Clamp(ljy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      lax = Clamp(lax, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      lay = Clamp(lay, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      laz = Clamp(laz, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      rjx = Clamp(rjx, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      rjy = Clamp(rjy, CHUK_JOYSTICK_MIN, CHUK_JOYSTICK_MAX);
      rax = Clamp(rax, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      ray = Clamp(ray, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      raz = Clamp(raz, CHUK_ACCELERO_MIN, CHUK_ACCELERO_MAX);
      // if all buttons are pressed, reset calibration
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
      // calibrate and sign the readings
      I8  sljx =  (I8)(ljx - g_zljx);
      I8  sljy =  (I8)(ljy - g_zljy);
      I16 slax = (I16)(lax - g_zlax);
      I16 slay = (I16)(lay - g_zlay);
      I16 slaz = (I16)(laz - g_zlaz);
      I8  srjx =  (I8)(rjx - g_zrjx);
      I8  srjy =  (I8)(rjy - g_zrjy);
      I16 srax = (I16)(rax - g_zrax);
      I16 sray = (I16)(ray - g_zray);
      I16 sraz = (I16)(raz - g_zraz);
      // convert the readings to float and scale them
      pInput->nLeftJoystickX  = (F32)sljx / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nLeftJoystickY  = (F32)sljy / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nLeftAcceleroX  = (F32)slax / (CHUK_ACCELERO_RANGE / 2);
      pInput->nLeftAcceleroY  = (F32)slay / (CHUK_ACCELERO_RANGE / 2);
      pInput->nLeftAcceleroZ  = (F32)slaz / (CHUK_ACCELERO_RANGE / 2);
      pInput->bLeftButtonZ    = lbz;
      pInput->bLeftButtonC    = lbc;
      pInput->nRightJoystickX = (F32)srjx / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nRightJoystickY = (F32)srjy / (CHUK_JOYSTICK_RANGE / 2);
      pInput->nRightAcceleroX = (F32)srax / (CHUK_ACCELERO_RANGE / 2);
      pInput->nRightAcceleroY = (F32)sray / (CHUK_ACCELERO_RANGE / 2);
      pInput->nRightAcceleroZ = (F32)sraz / (CHUK_ACCELERO_RANGE / 2);
      pInput->bRightButtonZ   = rbz;
      pInput->bRightButtonC   = rbc;
      return pInput;
   }
   return NULL;
}
