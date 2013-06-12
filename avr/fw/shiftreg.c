//===========================================================================
// Module:  shiftreg.c
// Purpose: AVR 74HC595 shift register driver
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
#include "shiftreg.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
static UI8  g_nShiftClockPin;             // shift register clock output (SHCP)
static UI8  g_nStoreClockPin;             // storage register clock output (STCP)
static UI8  g_nDataOutputPin;             // serial data output (DS)
static BYTE g_pbBuffer[SHIFTREG_SIZE];    // shift register buffer
//-----------< FUNCTION: WriteRegister >-------------------------------------
// Purpose:    writes the current buffer to the shift register
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister ()
{
   // shift in all bytes, LSB first
   for (UI8 i = 0; i < SHIFTREG_SIZE; i++)
   {
      // shift in all bits, MSB first
      for (I8 j = 7; j >= 0; j--)
      {
         // set the data register to the bit value
         // pulse SHCP to clock in the bit
         PinSet(g_nDataOutputPin, BitTest(g_pbBuffer[i], j));
         PinPulseHiLo(g_nShiftClockPin);
      }
   }
   // pulse STCP to latch in the register values
   PinPulseHiLo(g_nStoreClockPin);
}
//-----------< FUNCTION: ShiftRegInit >--------------------------------------
// Purpose:    initializes the shift register module
// Parameters: pConfig - shift register config
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegInit (PSHIFTREG_CONFIG pConfig)
{
   g_nShiftClockPin = pConfig->nShiftClockPin;
   g_nStoreClockPin = pConfig->nStoreClockPin;
   g_nDataOutputPin = pConfig->nDataOutputPin;
   PinSetOutput(g_nShiftClockPin);
   PinSetOutput(g_nStoreClockPin);
   PinSetOutput(g_nDataOutputPin);
   memzero(g_pbBuffer, SHIFTREG_SIZE);
   WriteRegister();
}
//-----------< FUNCTION: ShiftRegRead8 >-------------------------------------
// Purpose:    reads an 8-bit value from the shift register buffer
// Parameters: cbOffset - shift register offset, in bytes
// Returns:    the 8-bit value of the register at the byte offset
//---------------------------------------------------------------------------
UI8 ShiftRegRead8 (UI8 cbOffset)
{
   return g_pbBuffer[cbOffset];
}
//-----------< FUNCTION: ShiftRegWrite8 >------------------------------------
// Purpose:    writes an 8-bit value to the shift register
// Parameters: cbOffset - shift register offset, in bytes
//             nValue   - the 8-bit value to write at the offset
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegWrite8 (UI8 cbOffset, UI8 nValue)
{
   g_pbBuffer[cbOffset] = nValue;
   WriteRegister();
}
