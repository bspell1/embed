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
//-------------------[      Project Include Files      ]-------------------//
#include "sx1509.h"
#include "i2cmast.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
static const UI8 g_pI2cAddress[4] = { 0x3E, 0x3F, 0x70, 0x71 };
static BYTE g_pbI2cData[6];
static const UI8 g_pnTOnRegs[] = 
{
   SX1509_REG_TON0,
   SX1509_REG_TON1,
   SX1509_REG_TON2,
   SX1509_REG_TON3,
   SX1509_REG_TON4,
   SX1509_REG_TON5,
   SX1509_REG_TON6,
   SX1509_REG_TON7,
   SX1509_REG_TON8,
   SX1509_REG_TON9,
   SX1509_REG_TON10,
   SX1509_REG_TON11,
   SX1509_REG_TON12,
   SX1509_REG_TON13,
   SX1509_REG_TON14,
   SX1509_REG_TON15,
};
static const UI8 g_pnIOnRegs[] = 
{
   SX1509_REG_ION0,
   SX1509_REG_ION1,
   SX1509_REG_ION2,
   SX1509_REG_ION3,
   SX1509_REG_ION4,
   SX1509_REG_ION5,
   SX1509_REG_ION6,
   SX1509_REG_ION7,
   SX1509_REG_ION8,
   SX1509_REG_ION9,
   SX1509_REG_ION10,
   SX1509_REG_ION11,
   SX1509_REG_ION12,
   SX1509_REG_ION13,
   SX1509_REG_ION14,
   SX1509_REG_ION15,
};
static const UI8 g_pnOffRegs[] = 
{
   SX1509_REG_OFF0,
   SX1509_REG_OFF1,
   SX1509_REG_OFF2,
   SX1509_REG_OFF3,
   SX1509_REG_OFF4,
   SX1509_REG_OFF5,
   SX1509_REG_OFF6,
   SX1509_REG_OFF7,
   SX1509_REG_OFF8,
   SX1509_REG_OFF9,
   SX1509_REG_OFF10,
   SX1509_REG_OFF11,
   SX1509_REG_OFF12,
   SX1509_REG_OFF13,
   SX1509_REG_OFF14,
   SX1509_REG_OFF15,
};
static const UI8 g_pnTRiseRegs[] = 
{
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_TRISE4,
   SX1509_REG_TRISE5,
   SX1509_REG_TRISE6,
   SX1509_REG_TRISE7,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_TRISE12,
   SX1509_REG_TRISE13,
   SX1509_REG_TRISE14,
   SX1509_REG_TRISE15,
};
static const UI8 g_pnTFallRegs[] = 
{
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_TFALL4,
   SX1509_REG_TFALL5,
   SX1509_REG_TFALL6,
   SX1509_REG_TFALL7,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_INVALID,
   SX1509_REG_TFALL12,
   SX1509_REG_TFALL13,
   SX1509_REG_TFALL14,
   SX1509_REG_TFALL15,
};
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = 0;
   I2cSendRecv(g_pI2cAddress[nModule], g_pbI2cData, 1, g_pbI2cData + 1, 1);
   I2cWait();
   return g_pbI2cData[1];
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = nValue;
   I2cSend(g_pI2cAddress[nModule], g_pbI2cData, 2);
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = 0;
   g_pbI2cData[2] = 0;
   I2cSendRecv(g_pI2cAddress[nModule], g_pbI2cData, 1, g_pbI2cData + 1, 2);
   I2cWait();
   return 
      ((UI16)g_pbI2cData[1] << 8) | 
      (      g_pbI2cData[2] << 0);
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = (nValue >> 8);
   g_pbI2cData[2] = (nValue >> 0);
   I2cSend(g_pI2cAddress[nModule], g_pbI2cData, 3);
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = 0;
   g_pbI2cData[2] = 0;
   g_pbI2cData[3] = 0;
   g_pbI2cData[4] = 0;
   I2cSendRecv(g_pI2cAddress[nModule], g_pbI2cData, 1, g_pbI2cData + 1, 4);
   I2cWait();
   return 
      ((UI32)g_pbI2cData[1] << 24) | 
      ((UI32)g_pbI2cData[2] << 16) | 
      ((UI32)g_pbI2cData[3] <<  8) | 
      (      g_pbI2cData[4] <<  0);
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
   g_pbI2cData[0] = nReg;
   g_pbI2cData[1] = (nValue >> 24);
   g_pbI2cData[2] = (nValue >> 16);
   g_pbI2cData[3] = (nValue >>  8);
   g_pbI2cData[4] = (nValue >>  0);
   I2cSend(g_pI2cAddress[nModule], g_pbI2cData, 5);
}
//-----------< FUNCTION: SX1509GetKeyData >----------------------------------
// Purpose:    reads the keyboard data register to determine the key pressed
// Parameters: nModule - SX1509 module number
// Returns:    the current keyboard data register
//             -1, -1 if no key was pressed
//---------------------------------------------------------------------------
SX1509_KEYPAD_DATA SX1509GetKeyData (UI8 nModule)
{ 
   UI16 nData = SX1509Get16(nModule, SX1509_REG_KEYDATA1);
   UI8 nRowData = nData >> 8;
   UI8 nColData = nData;
   SX1509_KEYPAD_DATA data = { -1, -1 };
   for (UI8 i = 0; i < 8; i++)
      if (!(nRowData & BitMask(i)))
         data.nRow = i;
   for (UI8 i = 0; i < 8; i++)
      if (!(nColData & BitMask(i)))
         data.nCol = i + 8;
   return data;
}
//-----------< FUNCTION: SX1509GetOnTime >-----------------------------------
// Purpose:    reads the PWM on time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    the pin's PWM on time value
//---------------------------------------------------------------------------
UI8 SX1509GetOnTime (UI8 nModule, UI8 nIo)
{
   return SX1509Get8(nModule, g_pnTOnRegs[nIo]);
}
//-----------< FUNCTION: SX1509SetOnTime >-----------------------------------
// Purpose:    writes the PWM on time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             nValue  - the PWM on time value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetOnTime (UI8 nModule, UI8 nIo, UI8 nValue)
{
   return SX1509Set8(nModule, g_pnTOnRegs[nIo], nValue);
}
//-----------< FUNCTION: SX1509GetOnIntensity >------------------------------
// Purpose:    reads the PWM on intensity value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    the pin's PWM on intensity value
//---------------------------------------------------------------------------
UI8 SX1509GetOnIntensity (UI8 nModule, UI8 nIo)
{
   return SX1509Get8(nModule, g_pnIOnRegs[nIo]);
}
//-----------< FUNCTION: SX1509SetOnIntensity >------------------------------
// Purpose:    writes the PWM on intensity value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             nValue  - the PWM on intensity value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetOnIntensity (UI8 nModule, UI8 nIo, UI8 nValue)
{
   return SX1509Set8(nModule, g_pnIOnRegs[nIo], nValue);
}
//-----------< FUNCTION: SX1509GetOff >--------------------------------------
// Purpose:    reads the PWM "off" register for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    the pin's PWM "off" register value
//---------------------------------------------------------------------------
UI8 SX1509GetOff (UI8 nModule, UI8 nIo)
{
   return SX1509Get8(nModule, g_pnOffRegs[nIo]);
}
//-----------< FUNCTION: SX1509SetOff >--------------------------------------
// Purpose:    writes the PWM "off" register value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             nValue  - the PWM "off" register value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetOff (UI8 nModule, UI8 nIo, UI8 nValue)
{
   return SX1509Set8(nModule, g_pnOffRegs[nIo], nValue);
}
//-----------< FUNCTION: SX1509GetFadeInTime >-------------------------------
// Purpose:    reads the PWM fade-in time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    the pin's PWM fade-in time value
//---------------------------------------------------------------------------
UI8 SX1509GetFadeInTime (UI8 nModule, UI8 nIo)
{
   return SX1509Get8(nModule, g_pnTRiseRegs[nIo]);
}
//-----------< FUNCTION: SX1509SetFadeInTime >-------------------------------
// Purpose:    writes the PWM fade-in time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             nValue  - the PWM fade-in time value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetFadeInTime (UI8 nModule, UI8 nIo, UI8 nValue)
{
   return SX1509Set8(nModule, g_pnTRiseRegs[nIo], nValue);
}
//-----------< FUNCTION: SX1509GetFadeOutTime >------------------------------
// Purpose:    reads the PWM fade-out time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    the pin's PWM fade-out time value
//---------------------------------------------------------------------------
UI8 SX1509GetFadeOutTime (UI8 nModule, UI8 nIo)
{
   return SX1509Get8(nModule, g_pnTFallRegs[nIo]);
}
//-----------< FUNCTION: SX1509SetFadeOutTime >------------------------------
// Purpose:    writes the PWM fade-out time value for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             nValue  - the PWM fade-out time value to assign
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetFadeOutTime (UI8 nModule, UI8 nIo, UI8 nValue)
{
   return SX1509Set8(nModule, g_pnTFallRegs[nIo], nValue);
}
//-----------< FUNCTION: SX1509GetPwmConfig >--------------------------------
// Purpose:    retrieves all PWM configuration register values for an output
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
// Returns:    PWM configuration for the pin
//---------------------------------------------------------------------------
SX1509_PWM_CONFIG SX1509GetPwmConfig (UI8 nModule, UI8 nIo)
{
   I2cWait();
   g_pbI2cData[0] = g_pnTOnRegs[nIo];
   g_pbI2cData[1] = 0;
   g_pbI2cData[2] = 0;
   g_pbI2cData[3] = 0;
   g_pbI2cData[4] = 0;
   g_pbI2cData[5] = 0;
   if (g_pnTRiseRegs[nIo] != SX1509_REG_INVALID)
      I2cSendRecv(g_pI2cAddress[nModule], g_pbI2cData, 1, g_pbI2cData + 1, 5);
   else
      I2cSendRecv(g_pI2cAddress[nModule], g_pbI2cData, 1, g_pbI2cData + 1, 3);
   I2cWait();
   return (SX1509_PWM_CONFIG)
   {
      .nOnTime       = g_pbI2cData[1],
      .nOnIntensity  = g_pbI2cData[2],
      .nOffTime      = g_pbI2cData[3] >> 3,
      .nOffIntensity = g_pbI2cData[3] & 0x07,
      .nFadeInTime   = g_pbI2cData[4],
      .nFadeOutTime  = g_pbI2cData[5],
   };
}
//-----------< FUNCTION: SX1509SetPwmConfig >--------------------------------
// Purpose:    writes all PWM configuration for an output pin
// Parameters: nModule - SX1509 module number
//             nIo     - the number of the pin to read
//             config  - PWM configuration for the pin
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509SetPwmConfig (UI8 nModule, UI8 nIo, SX1509_PWM_CONFIG config)
{
   I2cWait();
   g_pbI2cData[0] = g_pnTOnRegs[nIo];
   g_pbI2cData[1] = config.nOnTime;
   g_pbI2cData[2] = config.nOnIntensity;
   g_pbI2cData[3] = (config.nOffTime << 3) | (config.nOffIntensity & 0x07);
   g_pbI2cData[4] = config.nFadeInTime;
   g_pbI2cData[5] = config.nFadeOutTime;
   if (g_pnTRiseRegs[nIo] != SX1509_REG_INVALID)
      I2cSend(g_pI2cAddress[nModule], g_pbI2cData, 6);
   else
      I2cSend(g_pI2cAddress[nModule], g_pbI2cData, 4);
}
//-----------< FUNCTION: SX1509Reset >---------------------------------------
// Purpose:    resets the SX1509 to default register values
// Parameters: nModule - SX1509 module number
// Returns:    none
//---------------------------------------------------------------------------
VOID SX1509Reset (UI8 nModule)
{ 
   SX1509Set8(nModule, SX1509_REG_RESET, 0x12);
   SX1509Set8(nModule, SX1509_REG_RESET, 0x34);
}
