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
static BYTE I2cData[6];
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
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
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = 0;
   I2cSendRecv(h1509.Handle, I2cData, 1, I2cData + 1, 1);
   I2cWait();
   return I2cData[1];
}
//-----------< FUNCTION: SX1509Set8 >----------------------------------------
// Purpose:    writes an 8-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set8 (HSX1509 h1509, UI8 nReg, UI8 nValue)
{
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = nValue;
   I2cSend(h1509.Handle, I2cData, 2);
}
//-----------< FUNCTION: SX1509Get16 >---------------------------------------
// Purpose:    reads a 16-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI16 SX1509Get16 (HSX1509 h1509, UI8 nReg)
{
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = 0;
   I2cData[2] = 0;
   I2cSendRecv(h1509.Handle, I2cData, 1, I2cData + 1, 2);
   I2cWait();
   return 
      ((UI16)I2cData[1] << 8) | 
      (      I2cData[2] << 0);
}
//-----------< FUNCTION: SX1509Set16 >----------------------------------------
// Purpose:    writes a 16-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set16 (HSX1509 h1509, UI8 nReg, UI16 nValue)
{
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = (nValue >> 8);
   I2cData[2] = (nValue >> 0);
   I2cSend(h1509.Handle, I2cData, 3);
}
//-----------< FUNCTION: SX1509Get32 >---------------------------------------
// Purpose:    reads a 32-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI32 SX1509Get32 (HSX1509 h1509, UI8 nReg)
{
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = 0;
   I2cData[2] = 0;
   I2cData[3] = 0;
   I2cData[4] = 0;
   I2cSendRecv(h1509.Handle, I2cData, 1, I2cData + 1, 4);
   I2cWait();
   return 
      ((UI32)I2cData[1] << 24) | 
      ((UI32)I2cData[2] << 16) | 
      ((UI32)I2cData[3] <<  8) | 
      (      I2cData[4] <<  0);
}
//-----------< FUNCTION: SX1509Set32 >----------------------------------------
// Purpose:    writes a 32-bit register
// Parameters: h1509 - SX1509 component handle
//             nReg  - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set32 (HSX1509 h1509, UI8 nReg, UI32 nValue)
{
   I2cWait();
   I2cData[0] = nReg;
   I2cData[1] = (nValue >> 24);
   I2cData[2] = (nValue >> 16);
   I2cData[3] = (nValue >>  8);
   I2cData[4] = (nValue >>  0);
   I2cSend(h1509.Handle, I2cData, 5);
}
//-----------< FUNCTION: SX1509GetKeyData >----------------------------------
// Purpose:    reads the keyboard data register to determine the key pressed
// Parameters: h1509 - SX1509 component handle
// Returns:    the current keyboard data register
//             -1, -1 if no key was pressed
//---------------------------------------------------------------------------
HSX1509_KEYPAD_DATA SX1509GetKeyData (HSX1509 h1509)
{ 
   UI16 nData = SX1509Get16(h1509, SX1509_REG_KEYDATA1);
   UI8 nRowData = nData >> 8;
   UI8 nColData = nData;
   HSX1509_KEYPAD_DATA data = { -1, -1 };
   for (UI8 i = 0; i < 8; i++)
      if (!(nRowData & BIT_MASK(i)))
         data.nRow = i;
   for (UI8 i = 0; i < 8; i++)
      if (!(nColData & BIT_MASK(i)))
         data.nCol = i + 8;
   return data;
}
