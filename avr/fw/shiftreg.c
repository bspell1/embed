//===========================================================================
// Module:  shiftreg.c
// Purpose: 74HC595 shift register driver
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
static UI8  g_nClockPin = PIN_INVALID;          // shift register clock output (SHCP)
static UI8  g_nLatchPin = PIN_INVALID;          // storage register clock output (STCP)
static UI8  g_nDataPin  = PIN_INVALID;          // serial data output (DS)
static BYTE g_pbBuffer[SHIFTREG_SIZE] = { 0, }; // shift register buffer
//-----------< FUNCTION: WriteRegister >-------------------------------------
// Purpose:    writes the current buffer to the shift register
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID WriteRegister ()
{
   ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
   {
      // serialize access to the shift register
      cli();
      // shift in all bytes, MSB first
      for (I8 i = SHIFTREG_SIZE - 1; i >= 0; i--)
      {
         // shift in all bits, MSB first
         for (I8 j = 7; j >= 0; j--)
         {
            // set the data register to the bit value
            // pulse SHCP to clock in the bit
            PinSet(g_nDataPin, BitTest(g_pbBuffer[i], j));
            PinPulseHiLo(g_nClockPin);
         }
      }
      // pulse STCP to latch in the register values
      PinPulseHiLo(g_nLatchPin);
   }
}
//-----------< FUNCTION: ShiftRegInit >--------------------------------------
// Purpose:    initializes the shift register module
// Parameters: pConfig - shift register config
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegInit (PSHIFTREG_CONFIG pConfig)
{
   g_nClockPin = pConfig->nClockPin;
   g_nLatchPin = pConfig->nLatchPin;
   g_nDataPin = pConfig->nDataPin;
   PinSetOutput(g_nClockPin);
   PinSetOutput(g_nLatchPin);
   PinSetOutput(g_nDataPin);
   WriteRegister();
}
//-----------< FUNCTION: ShiftRegRead >--------------------------------------
// Purpose:    reads the entire shift register
// Parameters: pbRegister - return the register contents via here
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegRead (PBYTE pbRegister)
{
   memcpy(pbRegister, g_pbBuffer, SHIFTREG_SIZE);
}
//-----------< FUNCTION: ShiftRegWrite >-------------------------------------
// Purpose:    writes the entire shift register
// Parameters: pbRegister - register data to write
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegWrite (PCBYTE pbRegister)
{
   memcpy(g_pbBuffer, pbRegister, SHIFTREG_SIZE);
   WriteRegister();
}
//-----------< FUNCTION: ShiftRegRead1 >-------------------------------------
// Purpose:    reads a bit value from the shift register buffer
// Parameters: nOffset - shift register offset, in bits
// Returns:    the bit value of the register at the specified offset
//---------------------------------------------------------------------------
BIT ShiftRegRead1 (UI8 nOffset)
{
   return (g_pbBuffer[nOffset / 8] >> (nOffset % 8)) & 0x1;
}
//-----------< FUNCTION: ShiftRegWrite1 >------------------------------------
// Purpose:    writes a bit value to the shift register
// Parameters: nOffset - shift register offset, in bits
//             bValue  - the bit value to write at the offset
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegWrite1 (UI8 nOffset, BIT bValue)
{

   g_pbBuffer[nOffset / 8] = bValue ? 
      g_pbBuffer[nOffset / 8] |  (1 << (nOffset % 8)) : 
      g_pbBuffer[nOffset / 8] & ~(1 << (nOffset % 8));
   WriteRegister();
}
//-----------< FUNCTION: ShiftRegRead4 >-------------------------------------
// Purpose:    reads a 4-bit value from the shift register buffer
// Parameters: nOffset - shift register offset, in nibbles
// Returns:    the 4-bit value of the register at the specified offset
//---------------------------------------------------------------------------
UI8 ShiftRegRead4 (UI8 nOffset)
{
   return nOffset % 2 == 0 ?
      g_pbBuffer[nOffset / 2] & 0x0F :
      g_pbBuffer[nOffset / 2] >> 4;
}
//-----------< FUNCTION: ShiftRegWrite4 >------------------------------------
// Purpose:    writes a 4-bit value to the shift register
// Parameters: nOffset - shift register offset, in nibbles
//             nValue  - the 4-bit value to write at the offset
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegWrite4 (UI8 nOffset, UI8 nValue)
{
   g_pbBuffer[nOffset / 2] = nOffset % 2 == 0 ? 
      (g_pbBuffer[nOffset / 2] & 0xF0) | (nValue & 0xF) :
      (g_pbBuffer[nOffset / 2] & 0x0F) | (nValue << 4);
   WriteRegister();
}
//-----------< FUNCTION: ShiftRegRead8 >-------------------------------------
// Purpose:    reads an 8-bit value from the shift register buffer
// Parameters: nOffset - shift register offset, in bytes
// Returns:    the 8-bit value of the register at the specified offset
//---------------------------------------------------------------------------
UI8 ShiftRegRead8 (UI8 nOffset)
{
   return g_pbBuffer[nOffset];
}
//-----------< FUNCTION: ShiftRegWrite8 >------------------------------------
// Purpose:    writes an 8-bit value to the shift register
// Parameters: nOffset - shift register offset, in bytes
//             nValue  - the 8-bit value to write at the offset
// Returns:    none
//---------------------------------------------------------------------------
VOID ShiftRegWrite8 (UI8 nOffset, UI8 nValue)
{
   g_pbBuffer[nOffset] = nValue;
   WriteRegister();
}
