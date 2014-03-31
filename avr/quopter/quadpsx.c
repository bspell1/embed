//===========================================================================
// Module:  quadpsx.c
// Purpose: quadcopter Playstation controller input receiver
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
#include "quadpsx.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
#define PSX_PACKETSIZE           6
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: QuadPsxInit >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadPsxInit (PQUADPSX_CONFIG pConfig)
{
   Nrf24SetPipeRXEnabled(pConfig->nPipe, TRUE);
   Nrf24SetRXAddress(pConfig->nPipe, pConfig->pszAddress);
   Nrf24SetPayloadLength(pConfig->nPipe, PSX_PACKETSIZE);
   Nrf24SetPipeAutoAck(pConfig->nPipe, FALSE);
}
//-----------< FUNCTION: QuadPsxBeginRead >----------------------------------
// Purpose:    starts an asynchronous chuk read operation
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadPsxBeginRead ()
{
   Nrf24PowerOn(NRF24_MODE_RECV);
}
//-----------< FUNCTION: QuadPsxEndRead >------------------------------------
// Purpose:    completes a chuk read operation
// Parameters: pInput - return the operation results via here
// Returns:    pInput if the input state was read
//             NULL otherwise
//---------------------------------------------------------------------------
PQUADPSX_INPUT QuadPsxEndRead (PQUADPSX_INPUT pInput)
{
   // attempt to receive a chuk packet from the NRF24
   if (Nrf24ClearIrq(NRF24_IRQ_ALL) & NRF24_IRQ_RX_DR)
   {
      BYTE pbPkt[PSX_PACKETSIZE];
      Nrf24Recv(pbPkt, PSX_PACKETSIZE);
      // decode the readings from the buffer
      BOOL bsl = !(pbPkt[0] & 0x01);         // byte 0[0] is !select button
      BOOL bst = !(pbPkt[0] & 0x08);         // byte 0[3] is !start button
      BOOL lb2 = !((pbPkt[1] >> 0) & 0x1);   // byte 1[0] is !left 2 button
      BOOL rb2 = !((pbPkt[1] >> 1) & 0x1);   // byte 1[1] is !right 2 button
      BOOL lb1 = !((pbPkt[1] >> 2) & 0x1);   // byte 1[2] is !left 1 button
      BOOL rb1 = !((pbPkt[1] >> 3) & 0x1);   // byte 1[3] is !right 1 button
      UI8  rjx = pbPkt[2];                   // byte 2 is right joystick X
      UI8  rjy = pbPkt[3];                   // byte 3 is right joystick Y
      UI8  ljx = pbPkt[4];                   // byte 4 is left joystick X
      UI8  ljy = pbPkt[5];                   // byte 5 is left joystick Y
      // convert the readings to float and translate/scale them
      pInput->bSelect = bsl;
      pInput->bStart  = bst;
      pInput->bL1     = lb1;
      pInput->bL2     = lb2;
      pInput->bR1     = rb1;
      pInput->bR2     = rb2;
      pInput->nLX     = ((F32)ljx - 127.5f) / 127.5f;
      pInput->nLY     = ((F32)ljy - 127.5f) / 127.5f;
      pInput->nRX     = ((F32)rjx - 127.5f) / 127.5f;
      pInput->nRY     = ((F32)rjy - 127.5f) / 127.5f;
      // desensitize joystick values
      if (fabs(pInput->nLX) < 0.15f) pInput->nLX = 0.0f;
      if (fabs(pInput->nLY) < 0.15f) pInput->nLY = 0.0f;
      if (fabs(pInput->nRX) < 0.15f) pInput->nRX = 0.0f;
      if (fabs(pInput->nRY) < 0.15f) pInput->nRY = 0.0f;
      return pInput;
   }
   return NULL;
}
