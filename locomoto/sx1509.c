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
static UI8  pI2cAddress[4] = { 0x3E, 0x3F, 0x70, 0x71 };
static BYTE pbI2cData[6];
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SX1509Init >----------------------------------------
// Purpose:    SX1509 interface initialization
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509Init ()
{
}
//-----------< FUNCTION: SX1509Get8 >----------------------------------------
// Purpose:    reads an 8-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI8 SX1509Get8 (UI8 nModule, UI8 nReg)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = 0;
   I2cSendRecv(pI2cAddress[nModule], pbI2cData, 1, pbI2cData + 1, 1);
   I2cWait();
   return pbI2cData[1];
}
//-----------< FUNCTION: SX1509Set8 >----------------------------------------
// Purpose:    writes an 8-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set8 (UI8 nModule, UI8 nReg, UI8 nValue)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = nValue;
   I2cSend(pI2cAddress[nModule], pbI2cData, 2);
}
//-----------< FUNCTION: SX1509Get16 >---------------------------------------
// Purpose:    reads a 16-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI16 SX1509Get16 (UI8 nModule, UI8 nReg)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = 0;
   pbI2cData[2] = 0;
   I2cSendRecv(pI2cAddress[nModule], pbI2cData, 1, pbI2cData + 1, 2);
   I2cWait();
   return 
      ((UI16)pbI2cData[1] << 8) | 
      (      pbI2cData[2] << 0);
}
//-----------< FUNCTION: SX1509Set16 >----------------------------------------
// Purpose:    writes a 16-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set16 (UI8 nModule, UI8 nReg, UI16 nValue)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = (nValue >> 8);
   pbI2cData[2] = (nValue >> 0);
   I2cSend(pI2cAddress[nModule], pbI2cData, 3);
}
//-----------< FUNCTION: SX1509Get32 >---------------------------------------
// Purpose:    reads a 32-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to read
// Returns:    the requested register value
//---------------------------------------------------------------------------
UI32 SX1509Get32 (UI8 nModule, UI8 nReg)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = 0;
   pbI2cData[2] = 0;
   pbI2cData[3] = 0;
   pbI2cData[4] = 0;
   I2cSendRecv(pI2cAddress[nModule], pbI2cData, 1, pbI2cData + 1, 4);
   I2cWait();
   return 
      ((UI32)pbI2cData[1] << 24) | 
      ((UI32)pbI2cData[2] << 16) | 
      ((UI32)pbI2cData[3] <<  8) | 
      (      pbI2cData[4] <<  0);
}
//-----------< FUNCTION: SX1509Set32 >----------------------------------------
// Purpose:    writes a 32-bit register
// Parameters: nModule - SX1509 module number
//             nReg    - the register to write
// Returns:    the requested register value
//---------------------------------------------------------------------------
VOID SX1509Set32 (UI8 nModule, UI8 nReg, UI32 nValue)
{
   I2cWait();
   pbI2cData[0] = nReg;
   pbI2cData[1] = (nValue >> 24);
   pbI2cData[2] = (nValue >> 16);
   pbI2cData[3] = (nValue >>  8);
   pbI2cData[4] = (nValue >>  0);
   I2cSend(pI2cAddress[nModule], pbI2cData, 5);
}
//-----------< FUNCTION: SX1509GetKeyData >----------------------------------
// Purpose:    reads the keyboard data register to determine the key pressed
// Parameters: nModule - SX1509 module number
// Returns:    the current keyboard data register
//             -1, -1 if no key was pressed
//---------------------------------------------------------------------------
UI8_KEYPAD_DATA SX1509GetKeyData (UI8 nModule)
{ 
   UI16 nData = SX1509Get16(nModule, SX1509_REG_KEYDATA1);
   UI8 nRowData = nData >> 8;
   UI8 nColData = nData;
   UI8_KEYPAD_DATA data = { -1, -1 };
   for (UI8 i = 0; i < 8; i++)
      if (!(nRowData & BIT_MASK(i)))
         data.nRow = i;
   for (UI8 i = 0; i < 8; i++)
      if (!(nColData & BIT_MASK(i)))
         data.nCol = i + 8;
   return data;
}
