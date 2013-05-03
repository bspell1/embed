//===========================================================================
// Module:  sx1509.c
// Purpose: AVR SX1509 GPIO expander driver
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
#include <alloca.h>
#include <string.h>
//-------------------[      Project Include Files      ]-------------------//
#include "sx1509.h"
#include "i2cmast.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: RegisterRead >--------------------------------------
// Purpose:    reads a value from a SX1509 register
// Parameters: h1509  - SX1509 component handle
//             pbData - pointer to read buffer, first byte is register
//             cbData - number of bytes to read
// Returns:    none
//---------------------------------------------------------------------------
static VOID RegisterRead (HSX1509 h1509, PBYTE pbData, BSIZE cbData)
{
   I2cSendRecv(h1509.Handle, pbData, 1, pbData + 1, cbData - 1);
   while (I2cIsBusy())
      ;
}
//-----------< FUNCTION: RegisterWrite >-------------------------------------
// Purpose:    writes a value to a SX1509 register
// Parameters: h1509  - SX1509 component handle
//             pbData - pointer to write buffer, first byte is register
//             cbData - number of bytes to write
// Returns:    none
//---------------------------------------------------------------------------
static VOID RegisterWrite (HSX1509 h1509, PBYTE pbData, BSIZE cbData)
{
   I2cSend(h1509.Handle, pbData, cbData);
}
//-----------< FUNCTION: SX1509Init >----------------------------------------
// Purpose:    SX1509 interface initialization
// Parameters: nAddress - I2C address of the chip
// Returns:    handle to the SX1509 interface
//---------------------------------------------------------------------------
HSX1509 SX1509Init (UI8 nAddress)
{
   // create the component handle
   HSX1509 h1509 = { nAddress };
   return h1509;
}
//-----------< FUNCTION: SX1509Get8 >----------------------------------------
// Purpose:    reads an 8-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI8 SX1509Get8 (HSX1509 h1509, UI8 nReg)
{
   BYTE data[2] = { nReg, 0 };
   RegisterRead(h1509, data, sizeof(data));
   return data[1];
}
//-----------< FUNCTION: SX1509Set8 >----------------------------------------
// Purpose:    writes an 8-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set8 (HSX1509 h1509, UI8 nReg, UI8 nValue)
{
   BYTE data[2] = { nReg, nValue };
   RegisterWrite(h1509, data, sizeof(data));
}
//-----------< FUNCTION: SX1509Get16 >---------------------------------------
// Purpose:    reads a 16-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI16 SX1509Get16 (HSX1509 h1509, UI8 nReg)
{
   BYTE data[3] = { nReg, 0, 0 };
   RegisterRead(h1509, data, sizeof(data));
   return 
      ((UI16)data[1] << 8) | 
      (      data[2] << 0);
}
//-----------< FUNCTION: SX1509Set16 >----------------------------------------
// Purpose:    writes a 16-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set16 (HSX1509 h1509, UI8 nReg, UI16 nValue)
{
   BYTE data[3] = { nReg, (nValue >> 8), (nValue >> 0) };
   RegisterWrite(h1509, data, sizeof(data));
}
//-----------< FUNCTION: SX1509Get32 >---------------------------------------
// Purpose:    reads a 32-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI32 SX1509Get32 (HSX1509 h1509, UI8 nReg)
{
   BYTE data[5] = { nReg, 0, 0, 0, 0 };
   RegisterRead(h1509, data, sizeof(data));
   return 
      ((UI32)data[1] << 24) | 
      ((UI32)data[2] << 16) | 
      ((UI32)data[3] <<  8) | 
      (      data[4] <<  0);
}
//-----------< FUNCTION: SX1509Set32 >----------------------------------------
// Purpose:    writes a 32-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set32 (HSX1509 h1509, UI8 nReg, UI32 nValue)
{
   BYTE data[5] = { nReg, (nValue >> 24), (nValue >> 16), (nValue >> 8), (nValue >> 0) };
   RegisterWrite(h1509, data, sizeof(data));
}
