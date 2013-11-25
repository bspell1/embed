//===========================================================================
// Module:  quadtel.c
// Purpose: quadcopter telemetrics transmitter
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
#include "quadtel.h"
#include "nrf24.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: QuadTelInit >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadTelInit (PQUADTEL_CONFIG pConfig)
{
   Nrf24DisableAck();
   Nrf24SetTXAddress(pConfig->pszAddress);
   Nrf24SetPipeAutoAck(NRF24_PIPE0, FALSE);
}
//-----------< FUNCTION: QuadTelSend >---------------------------------------
// Purpose:    transmits a telemetrics packet
// Parameters: pData - telemetrics to send
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadTelSend (PQUADTEL_DATA pData)
{
   Nrf24PowerOn(NRF24_MODE_SEND);
   Nrf24Send(pData, sizeof(*pData));
}
