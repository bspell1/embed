//===========================================================================
// Module:  sx1509.h
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
#ifndef __SX1509_H
#define __SX1509_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// SX1509 STRUCTURE
//===========================================================================
// IC instance handle
typedef struct tagHSX1509
{
   UI8 Handle;
} HSX1509;
// row/column configuration
typedef struct tagHSX1509KeypadDim
{
   UI8   nRows;
   UI8   nCols;
} HSX1509_KEYPAD_DIM;
// row/column data
typedef struct tagHSX1509KeypadData
{
   UI8   nRow;
   UI8   nCol;
} HSX1509_KEYPAD_DATA;
//===========================================================================
// SX1509 REGISTERS
//===========================================================================
// device and I/O banks
#define SX1509_REG_INPUTDISABLE           0x00
#define SX1509_REG_INPUTDISABLEB          0x00
#define SX1509_REG_INPUTDISABLEA          0x01
#define SX1509_REG_LONGSLEW               0x02
#define SX1509_REG_LONGSLEWB              0x02
#define SX1509_REG_LONGSLEWA              0x03
#define SX1509_REG_LOWDRIVE               0x04
#define SX1509_REG_LOWDRIVEB              0x04
#define SX1509_REG_LOWDRIVEA              0x05
#define SX1509_REG_PULLUP                 0x06
#define SX1509_REG_PULLUPB                0x06
#define SX1509_REG_PULLUPA                0x07
#define SX1509_REG_PULLDOWN               0x08
#define SX1509_REG_PULLDOWNB              0x08
#define SX1509_REG_PULLDOWNA              0x09
#define SX1509_REG_OPENDRAIN              0x0A
#define SX1509_REG_OPENDRAINB             0x0A
#define SX1509_REG_OPENDRAINA             0x0B
#define SX1509_REG_POLARITY               0x0C
#define SX1509_REG_POLARITYB              0x0C
#define SX1509_REG_POLARITYA              0x0D
#define SX1509_REG_DIR                    0x0E
#define SX1509_REG_DIRB                   0x0E
#define SX1509_REG_DIRA                   0x0F
#define SX1509_REG_DATA                   0x10
#define SX1509_REG_DATAB                  0x10
#define SX1509_REG_DATAA                  0x11
#define SX1509_REG_INTERRUPTMASK          0x12
#define SX1509_REG_INTERRUPTMASKB         0x12
#define SX1509_REG_INTERRUPTMASKA         0x13
#define SX1509_REG_SENSE                  0x14
#define SX1509_REG_SENSEHIGHB             0x14
#define SX1509_REG_SENSELOWB              0x15
#define SX1509_REG_SENSEHIGHA             0x16
#define SX1509_REG_SENSELOWA              0x17
#define SX1509_REG_INTERRUPTSOURCE        0x18
#define SX1509_REG_INTERRUPTSOURCEB       0x18
#define SX1509_REG_INTERRUPTSOURCEA       0x19
#define SX1509_REG_EVENTSTATUS            0x1A
#define SX1509_REG_EVENTSTATUSB           0x1A
#define SX1509_REG_EVENTSTATUSA           0x1B
#define SX1509_REG_LEVELSHIFTER           0x1C
#define SX1509_REG_LEVELSHIFTER1          0x1C
#define SX1509_REG_LEVELSHIFTER2          0x1D
#define SX1509_REG_CLOCK                  0x1E
#define SX1509_REG_MISC                   0x1F
#define SX1509_REG_PWMDRIVERENABLE        0x20
#define SX1509_REG_PWMDRIVERENABLEB       0x20
#define SX1509_REG_PWMDRIVERENABLEA       0x21
// debounce and keypad engine
#define SX1509_REG_DEBOUNCECONFIG         0x22
#define SX1509_REG_DEBOUNCEENABLE         0x23
#define SX1509_REG_DEBOUNCEENABLEB        0x23
#define SX1509_REG_DEBOUNCEENABLEA        0x24
#define SX1509_REG_KEYCONFIG1             0x25
#define SX1509_REG_KEYCONFIG2             0x26
#define SX1509_REG_KEYDATA1               0x27
#define SX1509_REG_KEYDATA2               0x28
// PWM driver (static, blinking, breathing)
#define SX1509_REG_TON0                   0x29
#define SX1509_REG_ION0                   0x2A
#define SX1509_REG_OFF0                   0x02
#define SX1509_REG_TON1                   0x2C
#define SX1509_REG_ION1                   0x2D
#define SX1509_REG_OFF1                   0x2E
#define SX1509_REG_TON2                   0x2F
#define SX1509_REG_ION2                   0x30
#define SX1509_REG_OFF2                   0x31
#define SX1509_REG_TON3                   0x32
#define SX1509_REG_ION3                   0x33
#define SX1509_REG_OFF3                   0x34
#define SX1509_REG_TON4                   0x35
#define SX1509_REG_ION4                   0x36
#define SX1509_REG_OFF4                   0x37
#define SX1509_REG_TRISE4                 0x38
#define SX1509_REG_TFALL4                 0x39
#define SX1509_REG_TON5                   0x3A
#define SX1509_REG_ION5                   0x3B
#define SX1509_REG_OFF5                   0x3C
#define SX1509_REG_TRISE5                 0x3D
#define SX1509_REG_TFALL5                 0x3E
#define SX1509_REG_TON6                   0x3F
#define SX1509_REG_ION6                   0x40
#define SX1509_REG_OFF6                   0x41
#define SX1509_REG_TRISE6                 0x42
#define SX1509_REG_TFALL6                 0x43
#define SX1509_REG_TON7                   0x44
#define SX1509_REG_ION7                   0x45
#define SX1509_REG_OFF7                   0x46
#define SX1509_REG_TRISE7                 0x47
#define SX1509_REG_TFALL7                 0x48
#define SX1509_REG_TON8                   0x49
#define SX1509_REG_ION8                   0x4A
#define SX1509_REG_OFF8                   0x4B
#define SX1509_REG_TON9                   0x4C
#define SX1509_REG_ION9                   0x4D
#define SX1509_REG_OFF9                   0x4E
#define SX1509_REG_TON10                  0x4F
#define SX1509_REG_ION10                  0x50
#define SX1509_REG_OFF10                  0x51
#define SX1509_REG_TON11                  0x52
#define SX1509_REG_ION11                  0x53
#define SX1509_REG_OFF11                  0x54
#define SX1509_REG_TON12                  0x55
#define SX1509_REG_ION12                  0x56
#define SX1509_REG_OFF12                  0x57
#define SX1509_REG_TRISE12                0x58
#define SX1509_REG_TFALL12                0x59
#define SX1509_REG_TON13                  0x5A
#define SX1509_REG_ION13                  0x5B
#define SX1509_REG_OFF13                  0x5C
#define SX1509_REG_TRISE13                0x5D
#define SX1509_REG_TFALL13                0x5E
#define SX1509_REG_TON14                  0x5F
#define SX1509_REG_ION14                  0x60
#define SX1509_REG_OFF14                  0x61
#define SX1509_REG_TRISE14                0x62
#define SX1509_REG_TFALL14                0x63
#define SX1509_REG_TON15                  0x64
#define SX1509_REG_ION15                  0x65
#define SX1509_REG_OFF15                  0x66
#define SX1509_REG_TRISE15                0x67
#define SX1509_REG_TFALL15                0x68
// miscellaneous
#define SX1509_REG_HIGHINPUT              0x69
#define SX1509_REG_HIGHINPUTB             0x69
#define SX1509_REG_HIGHINPUTA             0x6A
// software reset
#define SX1509_REG_RESET                  0x6B
//===========================================================================
// DIRECTION REGISTER VALUES
//===========================================================================
#define SX1509_DIR_OUTPUT                 BIT_LO
#define SX1509_DIR_INPUT                  BIT_HI
//===========================================================================
// SENSE REGISTER VALUES
//===========================================================================
#define SX1509_SENSE_MODE_NONE            0x00
#define SX1509_SENSE_MODE_RISING          0x01
#define SX1509_SENSE_MODE_FALLING         0x02
#define SX1509_SENSE_MODE_BOTH            0X03
//===========================================================================
// LEVEL SHIFTER REGISTER VALUES
//===========================================================================
// level shift pairs
#define SX1509_LEVELSHIFT_0_8             0x00
#define SX1509_LEVELSHIFT_1_9             0x01
#define SX1509_LEVELSHIFT_2_10            0x02
#define SX1509_LEVELSHIFT_3_11            0x03
#define SX1509_LEVELSHIFT_4_12            0x04
#define SX1509_LEVELSHIFT_5_13            0x05
#define SX1509_LEVELSHIFT_6_14            0x06
#define SX1509_LEVELSHIFT_7_15            0x07
// level shift modes
#define SX1509_LEVELSHIFT_MODE_OFF        0x00
#define SX1509_LEVELSHIFT_MODE_A2B        0x01
#define SX1509_LEVELSHIFT_MODE_B2A        0x02
//===========================================================================
// CLOCK REGISTER VALUES
//===========================================================================
// clock sources
#define SX1509_CLOCK_SOURCE_NONE          0x00
#define SX1509_CLOCK_SOURCE_OSCIN         0x01
#define SX1509_CLOCK_SOURCE_INTERNAL      0x02
// clock modes
#define SX1509_CLOCK_MODE_INPUT           BIT_LO
#define SX1509_CLOCK_MODE_OUTPUT          BIT_HI
// clock output frequency
#define SX1509_CLOCK_OSCOUT_GPO_0         0x00
#define SX1509_CLOCK_OSCOUT_GPO_1         0x0F
//===========================================================================
// MISCELLANEOUS REGISTER VALUES
//===========================================================================
// fader modes
#define SX1509_MISC_FADER_LINEAR          BIT_LO
#define SX1509_MISC_FADER_LOGARITHMIC     BIT_HI
// RESET modes
#define SX1509_MISC_RESET_HARD            BIT_LO
#define SX1509_MISC_RESET_SOFT            BIT_HI
//===========================================================================
// DEBOUNCE CONFIG REGISTER VALUES
//===========================================================================
#define SX1509_DEBOUNCE_0_5MSX            0x00
#define SX1509_DEBOUNCE_1MSX              0x01
#define SX1509_DEBOUNCE_2MSX              0x02
#define SX1509_DEBOUNCE_4MSX              0x03
#define SX1509_DEBOUNCE_8MSX              0x04
#define SX1509_DEBOUNCE_16MSX             0x05
#define SX1509_DEBOUNCE_32MSX             0x06
#define SX1509_DEBOUNCE_64MSX             0x07
//===========================================================================
// KEY CONFIG 1 REGISTER VALUES
//===========================================================================
// key scan time per row
#define SX1509_SCAN_1MSX                  0x00
#define SX1509_SCAN_2MSX                  0x01
#define SX1509_SCAN_4MSX                  0x02
#define SX1509_SCAN_8mSX                  0x03
#define SX1509_SCAN_16MSX                 0x04
#define SX1509_SCAN_32MSX                 0x05
#define SX1509_SCAN_64MSX                 0x06
#define SX1509_SCAN_128MSX                0x07
// auto sleep time
#define SX1509_AUTOSLEEP_OFF              0x00
#define SX1509_AUTOSLEEP_128MSX           0x01
#define SX1509_AUTOSLEEP_256MSX           0x02
#define SX1509_AUTOSLEEP_512MSX           0x03
#define SX1509_AUTOSLEEP_1SX              0x04
#define SX1509_AUTOSLEEP_2SX              0x05
#define SX1509_AUTOSLEEP_4SX              0x06
#define SX1509_AUTOSLEEP_8SX              0x07
//===========================================================================
// SX1509 CORE API
//===========================================================================
// API initialiization (addresses include 0x3E, 0x3F, 0x70, 0x71)
HSX1509  SX1509Init        (UI8 nAddress);
// register read/write
UI8      SX1509Get8        (HSX1509 h1509, UI8 nReg);
VOID     SX1509Set8        (HSX1509 h1509, UI8 nReg, UI8 nValue);
UI16     SX1509Get16       (HSX1509 h1509, UI8 nReg);
VOID     SX1509Set16       (HSX1509 h1509, UI8 nReg, UI16 nValue);
UI32     SX1509Get32       (HSX1509 h1509, UI8 nReg);
VOID     SX1509Set32       (HSX1509 h1509, UI8 nReg, UI32 nValue);
//===========================================================================
// REGISTER BIT MANIPULATION
//===========================================================================
inline BIT SX1509GetBit (HSX1509 h1509, UI8 nReg, UI8 nBit)
   { return BIT_TEST(SX1509Get8(h1509, nReg), nBit); }
inline VOID SX1509SetHi (HSX1509 h1509, UI8 nReg, UI8 nBit)
   { SX1509Set8(h1509, nReg, SX1509Get8(h1509, nReg) | BIT_MASK(nBit)); }
inline VOID SX1509SetLo (HSX1509 h1509, UI8 nReg, UI8 nBit)
   { SX1509Set8(h1509, nReg, SX1509Get8(h1509, nReg) & ~BIT_MASK(nBit)); }
inline VOID SX1509SetBit (HSX1509 h1509, UI8 nReg, UI8 nBit, BOOL fValue)
   {  if (fValue) 
         SX1509SetHi(h1509, nReg, nBit); 
      else 
         SX1509SetLo(h1509, nReg, nBit); 
   }
inline VOID SX1509Toggle (HSX1509 h1509, UI8 nReg, UI8 nBit)
   { SX1509Set8(h1509, nReg, SX1509Get8(h1509, nReg) ^ BIT_MASK(nBit)); }
//===========================================================================
// INPUT DISABLE REGISTER
//===========================================================================
inline UI16 SX1509GetInputDisable (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_INPUTDISABLE); }
inline VOID SX1509SetInputDisable (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_INPUTDISABLE, nValue); }
inline UI8 SX1509GetInputDisableA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INPUTDISABLEA); }
inline VOID SX1509SetInputDisableA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INPUTDISABLEA, nValue); }
inline UI8 SX1509GetInputDisableB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INPUTDISABLEB); }
inline VOID SX1509SetInputDisableB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INPUTDISABLEB, nValue); }
inline BIT SX1509GetInputDisableBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_INPUTDISABLEA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_INPUTDISABLEA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_INPUTDISABLEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInputDisable (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
//===========================================================================
// LONG SLEW REGISTER
//===========================================================================
inline UI16 SX1509GetLongSlew (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_LONGSLEW); }
inline VOID SX1509SetLongSlew (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_LONGSLEW, nValue); }
inline UI8 SX1509GetLongSlewA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LONGSLEWA); }
inline VOID SX1509SetLongSlewA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LONGSLEWA, nValue); }
inline UI8 SX1509GetLongSlewB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LONGSLEWB); }
inline VOID SX1509SetLongSlewB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LONGSLEWB, nValue); }
inline BIT SX1509GetLongSlewBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_LONGSLEWA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_LONGSLEWA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_LONGSLEWB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleLongSlew (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_LONGSLEWB, nIo - 8);
   }
//===========================================================================
// LOW DRIVE REGISTER
//===========================================================================
inline UI16 SX1509GetLowDrive (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_LOWDRIVE); }
inline VOID SX1509SetLowDrive (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_LOWDRIVE, nValue); }
inline UI8 SX1509GetLowDriveA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LOWDRIVEA); }
inline VOID SX1509SetLowDriveA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LOWDRIVEA, nValue); }
inline UI8 SX1509GetLowDriveB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LOWDRIVEB); }
inline VOID SX1509SetLowDriveB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LOWDRIVEB, nValue); }
inline BIT SX1509GetLowDriveBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_LOWDRIVEA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_LOWDRIVEA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_LOWDRIVEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleLowDrive (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
//===========================================================================
// PULL-UP REGISTER
//===========================================================================
inline UI16 SX1509GetPullUp (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_PULLUP); }
inline VOID SX1509SetPullUp (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_PULLUP, nValue); }
inline UI8 SX1509GetPullUpA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PULLUPA); }
inline VOID SX1509SetPullUpA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PULLUPA, nValue); }
inline UI8 SX1509GetPullUpB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PULLUPB); }
inline VOID SX1509SetPullUpB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PULLUPB, nValue); }
inline BIT SX1509GetPullUpBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_PULLUPA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_PULLUPA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_PULLUPA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_PULLUPA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_PULLUPB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePullUp (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_PULLUPA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_PULLUPB, nIo - 8);
   }
//===========================================================================
// PULL-DOWN REGISTER
//===========================================================================
inline UI16 SX1509GetPullDown (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_PULLDOWN); }
inline VOID SX1509SetPullDown (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_PULLDOWN, nValue); }
inline UI8 SX1509GetPullDownA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PULLDOWNA); }
inline VOID SX1509SetPullDownA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PULLDOWNA, nValue); }
inline UI8 SX1509GetPullDownB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PULLDOWNB); }
inline VOID SX1509SetPullDownB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PULLDOWNB, nValue); }
inline BIT SX1509GetPullDownIo (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_PULLDOWNA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownIo (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_PULLDOWNA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_PULLDOWNB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePullDown (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_PULLDOWNB, nIo - 8);
   }
//===========================================================================
// OPEN DRAIN REGISTER
//===========================================================================
inline UI16 SX1509GetOpenDrain (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_OPENDRAIN); }
inline VOID SX1509SetOpenDrain (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_OPENDRAIN, nValue); }
inline UI8 SX1509GetOpenDrainA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_OPENDRAINA); }
inline VOID SX1509SetOpenDrainA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_OPENDRAINA, nValue); }
inline UI8 SX1509GetOpenDrainB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_OPENDRAINB); }
inline VOID SX1509SetOpenDrainB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_OPENDRAINB, nValue); }
inline BIT SX1509GetOpenDrainIo (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_OPENDRAINA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainIo (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_OPENDRAINA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_OPENDRAINB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleOpenDrain (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_OPENDRAINB, nIo - 8);
   }
//===========================================================================
// POLARITY REGISTER
//===========================================================================
inline UI16 SX1509GetPolarity (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_POLARITY); }
inline VOID SX1509SetPolarity (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_POLARITY, nValue); }
inline UI8 SX1509GetPolarityA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_POLARITYA); }
inline VOID SX1509SetPolarityA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_POLARITYA, nValue); }
inline UI8 SX1509GetPolarityB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_POLARITYB); }
inline VOID SX1509SetPolarityB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_POLARITYB, nValue); }
inline BIT SX1509GetPolarityBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_POLARITYA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_POLARITYA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_POLARITYA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_POLARITYA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_POLARITYB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePolarity (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_POLARITYA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_POLARITYB, nIo - 8);
   }
//===========================================================================
// DIRECTION REGISTER
//===========================================================================
inline UI16 SX1509GetDir (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_DIR); }
inline VOID SX1509SetDir (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_DIR, nValue); }
inline UI8 SX1509GetDirA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DIRA); }
inline VOID SX1509SetDirA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DIRA, nValue); }
inline UI8 SX1509GetDirB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DIRB); }
inline VOID SX1509SetDirB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DIRB, nValue); }
inline BIT SX1509GetDirBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_DIRA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_DIRA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_DIRA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_DIRA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_DIRB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleDir (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_DIRA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirOutput (HSX1509 h1509, UI8 nIo)
   { SX1509SetDirBit(h1509, nIo, SX1509_DIR_OUTPUT); }
inline VOID SX1509SetDirInput (HSX1509 h1509, UI8 nIo)
   { SX1509SetDirBit(h1509, nIo, SX1509_DIR_INPUT); }
//===========================================================================
// DATA REGISTER
//===========================================================================
inline UI16 SX1509GetData (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_DATA); }
inline VOID SX1509SetData (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_DATA, nValue); }
inline UI8 SX1509GetDataA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DATAA); }
inline VOID SX1509SetDataA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DATAA, nValue); }
inline UI8 SX1509GetDataB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DATAB); }
inline VOID SX1509SetDataB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DATAB, nValue); }
inline BIT SX1509GetDataBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_DATAA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_DATAA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_DATAA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_DATAA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_DATAB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleData (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_DATAA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_DATAB, nIo - 8);
   }
//===========================================================================
// INTERRUPT MASK REGISTER
//===========================================================================
inline UI16 SX1509GetInterruptMask (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_INTERRUPTMASK); }
inline VOID SX1509SetInterruptMask (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_INTERRUPTMASK, nValue); }
inline UI8 SX1509GetInterruptMaskA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INTERRUPTMASKA); }
inline VOID SX1509SetInterruptMaskA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INTERRUPTMASKA, nValue); }
inline UI8 SX1509GetInterruptMaskB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INTERRUPTMASKB); }
inline VOID SX1509SetInterruptMaskB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INTERRUPTMASKB, nValue); }
inline BIT SX1509GetInterruptMaskBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_INTERRUPTMASKA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_INTERRUPTMASKA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_INTERRUPTMASKB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInterruptMask (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
//===========================================================================
// SENSE REGISTER
//===========================================================================
inline UI32 SX1509GetSense (HSX1509 h1509)
   { return SX1509Get32(h1509, SX1509_REG_SENSE); }
inline VOID SX1509SetSense (HSX1509 h1509, UI32 nValue)
   { SX1509Set32(h1509, SX1509_REG_SENSE, nValue); }
inline UI8 SX1509GetSenseLowA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_SENSELOWA); }
inline VOID SX1509SetSenseLowA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_SENSELOWA, nValue); }
inline UI8 SX1509GetSenseHighA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_SENSEHIGHA); }
inline VOID SX1509SetSenseHighA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_SENSEHIGHA, nValue); }
inline UI8 SX1509GetSenseLowB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_SENSELOWB); }
inline VOID SX1509SetSenseLowB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_SENSELOWB, nValue); }
inline UI8 SX1509GetSenseHighB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_SENSEHIGHB); }
inline VOID SX1509SetSenseHighB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_SENSEHIGHB, nValue); }
inline UI8 SX1509GetSenseMode (HSX1509 h1509, UI8 nIo)
   {  
      UI8 nShift = (2 * (nIo % 4));
      if (nIo < 4)
         return (SX1509GetSenseLowA(h1509) >> nShift) & 3;
      else if (nIo < 8)
         return (SX1509GetSenseHighA(h1509) >> nShift) & 3;
      else if (nIo < 12)
         return (SX1509GetSenseLowB(h1509) >> nShift) & 3;
      else
         return (SX1509GetSenseHighB(h1509) >> nShift) & 3;
   }
inline VOID SX1509SetSenseMode (HSX1509 h1509, UI8 nIo, UI8 nMode)
   { 
      UI8 nShift = (2 * (nIo % 4));
      UI8 nMask = 3 << nShift;
      nMode = (nMode << nShift) & nMask;
      if (nIo < 4)
         SX1509SetSenseLowA(h1509, (SX1509GetSenseLowA(h1509) & nMask) | nMode);
      else if (nIo < 8)
         SX1509SetSenseHighA(h1509, (SX1509GetSenseHighA(h1509) & nMask) | nMode);
      else if (nIo < 12)
         SX1509SetSenseLowB(h1509, (SX1509GetSenseLowB(h1509) & nMask) | nMode);
      else
         SX1509SetSenseHighB(h1509, (SX1509GetSenseHighB(h1509) & nMask) | nMode);
   }
inline VOID SX1509SetSenseNone (HSX1509 h1509, UI8 nIo)
   { SX1509SetSenseMode(h1509, nIo, SX1509_SENSE_MODE_NONE); }
inline VOID SX1509SetSenseRising (HSX1509 h1509, UI8 nIo)
   { SX1509SetSenseMode(h1509, nIo, SX1509_SENSE_MODE_RISING); }
inline VOID SX1509SetSenseFalling (HSX1509 h1509, UI8 nIo)
   { SX1509SetSenseMode(h1509, nIo, SX1509_SENSE_MODE_FALLING); }
inline VOID SX1509SetSenseBoth (HSX1509 h1509, UI8 nIo)
   { SX1509SetSenseMode(h1509, nIo, SX1509_SENSE_MODE_BOTH); }
//===========================================================================
// INTERRUPT SOURCE REGISTER
//===========================================================================
inline UI16 SX1509GetInterruptSource (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_INTERRUPTSOURCE); }
inline VOID SX1509SetInterruptSource (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_INTERRUPTSOURCE, nValue); }
inline UI8 SX1509GetInterruptSourceA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INTERRUPTSOURCEA); }
inline VOID SX1509SetInterruptSourceA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INTERRUPTSOURCEA, nValue); }
inline UI8 SX1509GetInterruptSourceB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_INTERRUPTSOURCEB); }
inline VOID SX1509SetInterruptSourceB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_INTERRUPTSOURCEB, nValue); }
inline BIT SX1509GetInterruptSourceBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_INTERRUPTSOURCEA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_INTERRUPTSOURCEA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_INTERRUPTSOURCEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInterruptSource (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
//===========================================================================
// EVENT STATUS REGISTER
//===========================================================================
inline UI16 SX1509GetEventStatus (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_EVENTSTATUS); }
inline VOID SX1509SetEventStatus (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_EVENTSTATUS, nValue); }
inline UI8 SX1509GetEventStatusA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_EVENTSTATUSA); }
inline VOID SX1509SetEventStatusA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_EVENTSTATUSA, nValue); }
inline UI8 SX1509GetEventStatusB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_EVENTSTATUSB); }
inline VOID SX1509SetEventStatusB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_EVENTSTATUSB, nValue); }
inline BIT SX1509GetEventStatusBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_EVENTSTATUSA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_EVENTSTATUSA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_EVENTSTATUSB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleEventStatus (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
//===========================================================================
// LEVEL SHIFT REGISTER
//===========================================================================
inline UI16 SX1509GetLevelShift (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_LEVELSHIFTER); }
inline VOID SX1509SetLevelShift (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_LEVELSHIFTER, nValue); }
inline UI8 SX1509GetLevelShift1 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LEVELSHIFTER1); }
inline VOID SX1509SetLevelShift1 (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LEVELSHIFTER1, nValue); }
inline UI8 SX1509GetLevelShift2 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_LEVELSHIFTER2); }
inline VOID SX1509SetLevelShift2 (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_LEVELSHIFTER2, nValue); }
inline UI8 SX1509GetLevelShiftMode (HSX1509 h1509, UI8 nPair)
   {  
      UI8 nShift = (2 * (nPair % 4));
      if (nPair < 4)
         return (SX1509GetLevelShift2(h1509) >> nShift) & 3;
      else
         return (SX1509GetLevelShift1(h1509) >> nShift) & 3;
   }
inline VOID SX1509SetLevelShiftMode (HSX1509 h1509, UI8 nPair, UI8 nMode)
   { 
      UI8 nShift = (2 * (nPair % 4));
      UI8 nMask = 3 << nShift;
      nMode = (nMode << nShift) & nMask;
      if (nPair < 4)
         SX1509SetLevelShift2(h1509, (SX1509GetLevelShift1(h1509) & nMask) | nMode);
      else
         SX1509SetLevelShift1(h1509, (SX1509GetLevelShift2(h1509) & nMask) | nMode);
   }
inline VOID SX1509SetLevelShiftOff (HSX1509 h1509, UI8 nPair)
   { SX1509SetLevelShiftMode(h1509, nPair, SX1509_LEVELSHIFT_MODE_OFF); }
inline VOID SX1509SetLevelShiftA2B (HSX1509 h1509, UI8 nPair)
   { SX1509SetLevelShiftMode(h1509, nPair, SX1509_LEVELSHIFT_MODE_A2B); }
inline VOID SX1509SetLevelShiftB2A (HSX1509 h1509, UI8 nPair)
   { SX1509SetLevelShiftMode(h1509, nPair, SX1509_LEVELSHIFT_MODE_B2A); }
//===========================================================================
// CLOCK REGISTER
//===========================================================================
inline UI8 SX1509GetClock (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_CLOCK); }
inline VOID SX1509SetClock (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_CLOCK, nValue); }
// output frequency bits (0-3)
inline UI8 SX1509GetClockFrequency (HSX1509 h1509)
   { return SX1509GetClock(h1509) & 0x07; }
inline VOID SX1509SetClockFrequency (HSX1509 h1509, UI8 nFrequency)
   { SX1509SetClock(h1509, (SX1509GetClock(h1509) & 0x07) | nFrequency); }
inline BIT SX1509GetClockGpo (HSX1509 h1509)
   { return (SX1509GetClockFrequency(h1509) == 0x0F) ? BIT_HI : BIT_LO; }
inline VOID SX1509SetClockGpo (HSX1509 h1509, BIT fValue)
   { SX1509SetClockFrequency(h1509, fValue ? 0x0F : 0x00); }
inline VOID SX1509SetClockGpoLo (HSX1509 h1509)
   { SX1509SetClockFrequency(h1509, 0x00); }
inline VOID SX1509SetClockGpoHi (HSX1509 h1509)
   { SX1509SetClockFrequency(h1509, 0x0F); }
inline VOID SX1509ToggleClockGpo (HSX1509 h1509)
   { SX1509SetClock(h1509, SX1509GetClock(h1509) ^ 0x7); }
// mode bit (4)
inline BIT SX1509GetClockMode (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_CLOCK, 4); }
inline VOID SX1509SetClockMode (HSX1509 h1509, BIT fMode)
   { SX1509SetBit(h1509, SX1509_REG_CLOCK, 4, fMode); }
inline VOID SX1509SetClockModeInput (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_CLOCK, 4, SX1509_CLOCK_MODE_INPUT); }
inline VOID SX1509SetClockModeOutput (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_CLOCK, 4, SX1509_CLOCK_MODE_OUTPUT); }
// oscillator frequency bits (5-6)
inline UI8 SX1509GetClockSource (HSX1509 h1509)
   { return (SX1509GetClock(h1509) >> 5) & 3; }
inline VOID SX1509SetClockSource (HSX1509 h1509, UI8 nSource)
   { SX1509SetClock(h1509, (SX1509GetClock(h1509) & 0x60) | (nSource << 5)); }
inline VOID SX1509SetClockSourceNone (HSX1509 h1509)
   { SX1509SetClockSource(h1509, SX1509_CLOCK_SOURCE_NONE); }
inline VOID SX1509SetClockSourceOscIn (HSX1509 h1509)
   { SX1509SetClockSource(h1509, SX1509_CLOCK_SOURCE_OSCIN); }
inline VOID SX1509SetClockSourceInternal (HSX1509 h1509)
   { SX1509SetClockSource(h1509, SX1509_CLOCK_SOURCE_INTERNAL); }
//===========================================================================
// MISCELLANEOUS REGISTER
//===========================================================================
inline UI8 SX1509GetMisc (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_MISC); }
inline VOID SX1509SetMisc (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_MISC, nValue); }
// auto-clear interrupt bit (0)
inline BOOL SX1509GetMiscInterruptAutoClear (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_MISC, 0); }
inline VOID SX1509SetMiscInterruptAutoClear (HSX1509 h1509, BOOL fValue)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 0, fValue); }
// auto-increment address BOOL (1)
inline BOOL SX1509GetMiscAddressAutoIncrement (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_MISC, 1); }
inline VOID SX1509SetMiscAddressAutoIncrement (HSX1509 h1509, BOOL fValue)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 1, fValue); }
// RESET function bit (2)
inline BIT SX1509GetMiscResetMode (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_MISC, 2); }
inline VOID SX1509SetMiscResetMode (HSX1509 h1509, BIT fValue)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 2, fValue); }
inline VOID SX1509SetMiscResetHard (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 2, SX1509_MISC_RESET_HARD); }
inline VOID SX1509SetMiscResetSoft (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 2, SX1509_MISC_RESET_SOFT); }
// fader mode bit for bank A (3)
inline BIT SX1509GetMiscFaderAMode (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_MISC, 3); }
inline VOID SX1509SetMiscFaderAMode (HSX1509 h1509, BIT fValue)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 3, fValue); }
inline VOID SX1509SetMiscFaderALinear (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 3, SX1509_MISC_FADER_LINEAR); }
inline VOID SX1509SetMiscFaderALogarithmic (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 3, SX1509_MISC_FADER_LOGARITHMIC); }
// PWM frequency bits (4-6)
inline UI8 SX1509GetMiscPwmFrequency (HSX1509 h1509)
   { return (SX1509GetMisc(h1509) >> 4) & 7; }
inline VOID SX1509SetMiscPwmFrequency (HSX1509 h1509, UI8 nFreq)
   { SX1509SetMisc(h1509, (SX1509GetMisc(h1509) & 0x70) | (nFreq << 4)); }
inline VOID SX1509SetMiscPwmOff (HSX1509 h1509)
   { SX1509SetMiscPwmFrequency(h1509, 0); }
// fader mode bit for bank B (7)
inline BIT SX1509GetMiscFaderBMode (HSX1509 h1509)
   { return SX1509GetBit(h1509, SX1509_REG_MISC, 7); }
inline VOID SX1509SetMiscFaderBMode (HSX1509 h1509, BIT fValue)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 7, fValue); }
inline VOID SX1509SetMiscFaderBLinear (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 7, SX1509_MISC_FADER_LINEAR); }
inline VOID SX1509SetMiscFaderBLogarithmic (HSX1509 h1509)
   { SX1509SetBit(h1509, SX1509_REG_MISC, 7, SX1509_MISC_FADER_LOGARITHMIC); }
//===========================================================================
// PWM DRIVER ENABLE REGISTER
//===========================================================================
inline UI16 SX1509GetPwmEnable (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_PWMDRIVERENABLE); }
inline VOID SX1509SetPwmEnable (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_PWMDRIVERENABLE, nValue); }
inline UI8 SX1509GetPwmEnableA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PWMDRIVERENABLEA); }
inline VOID SX1509SetPwmEnableA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PWMDRIVERENABLEA, nValue); }
inline UI8 SX1509GetPwmEnableB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_PWMDRIVERENABLEB); }
inline VOID SX1509SetPwmEnableB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_PWMDRIVERENABLEB, nValue); }
inline BIT SX1509GetPwmEnableBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_PWMDRIVERENABLEA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_PWMDRIVERENABLEA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_PWMDRIVERENABLEB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePwmEnable (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
//===========================================================================
// DEBOUNCE CONFIGURATION REGISTER
//===========================================================================
inline UI8 SX1509GetDebounceConfig (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DEBOUNCECONFIG); }
inline VOID SX1509SetDebounceConfig (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DEBOUNCECONFIG, nValue); }
// debounce time bits (0-2)   
inline UI8 SX1509GetDebounceTime (HSX1509 h1509)
   { return SX1509GetDebounceConfig(h1509) & 0x07; }
inline VOID SX1509SetDebounceTime (HSX1509 h1509, UI8 nValue)
   { 
      SX1509SetDebounceConfig(
         h1509, 
         (SX1509GetDebounceConfig(h1509) & 0xF8) | (nValue & 0x07)
      ); 
   }
//===========================================================================
// DEBOUNCE ENABLE REGISTER
//===========================================================================
inline UI16 SX1509GetDebounceEnable (HSX1509 h1509)
   { return SX1509Get16(h1509, SX1509_REG_DEBOUNCEENABLE); }
inline VOID SX1509SetDebounceEnable (HSX1509 h1509, UI16 nValue)
   { SX1509Set16(h1509, SX1509_REG_DEBOUNCEENABLE, nValue); }
inline UI8 SX1509GetDebounceEnableA (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DEBOUNCEENABLEA); }
inline VOID SX1509SetDebounceEnableA (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DEBOUNCEENABLEA, nValue); }
inline UI8 SX1509GetDebounceEnableB (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_DEBOUNCEENABLEB); }
inline VOID SX1509SetDebounceEnableB (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_DEBOUNCEENABLEB, nValue); }
inline BIT SX1509GetDebounceEnableBit (HSX1509 h1509, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(h1509, SX1509_REG_DEBOUNCEENABLEA, nIo) : 
         SX1509GetBit(h1509, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableLo (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(h1509, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509SetLo(h1509, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableHi (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(h1509, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509SetHi(h1509, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableBit (HSX1509 h1509, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(h1509, SX1509_REG_DEBOUNCEENABLEA, nIo, fValue);
      else
         SX1509SetBit(h1509, SX1509_REG_DEBOUNCEENABLEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleDebounceEnable (HSX1509 h1509, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(h1509, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509Toggle(h1509, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
//===========================================================================
// KEYPAD CONFIGURATION REGISTER 1
//===========================================================================
inline UI8 SX1509GetKeyConfig1 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_KEYCONFIG1); }
inline VOID SX1509SetKeyConfig1 (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_KEYCONFIG1, nValue); }
// scan time per row bits (0-2)
inline UI8 SX1509GetScanTime (HSX1509 h1509)
   { return SX1509GetKeyConfig1(h1509) & 0x07; }
inline VOID SX1509SetScanTime (HSX1509 h1509, UI8 nValue)
   { 
      SX1509SetKeyConfig1(
         h1509, 
         (SX1509GetKeyConfig1(h1509) & 0xF8) | (nValue & 0x07)
      ); 
   }
// auto sleep time bits (4-6)
inline UI8 SX1509GetScanAutoSleepTime (HSX1509 h1509)
   { return (SX1509GetKeyConfig1(h1509) & 0x70) >> 4; }
inline VOID SX1509SetScanAutoSleepTime (HSX1509 h1509, UI8 nValue)
   { 
      SX1509SetKeyConfig1(
         h1509, 
         (SX1509GetKeyConfig1(h1509) & 0x8F) | ((nValue & 0x07) << 4)
      ); 
   }
//===========================================================================
// KEYPAD CONFIGURATION REGISTER 2
//===========================================================================
inline UI8 SX1509GetKeyConfig2 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_KEYCONFIG2); }
inline VOID SX1509SetKeyConfig2 (HSX1509 h1509, UI8 nValue)
   { SX1509Set8(h1509, SX1509_REG_KEYCONFIG2, nValue); }
// column count bits (0-2)
inline UI8 SX1509GetKeypadCols (HSX1509 h1509)
   { return SX1509GetKeyConfig2(h1509) & 0x07; }
inline VOID SX1509SetKeypadCols (HSX1509 h1509, UI8 nValue)
   { 
      SX1509SetKeyConfig2(
         h1509, 
         (SX1509GetKeyConfig2(h1509) & 0xF8) | (nValue & 0x07)
      ); 
   }
// row count bits (3-5)
inline UI8 SX1509GetKeypadRows (HSX1509 h1509)
   { return (SX1509GetKeyConfig2(h1509) & 0x38) >> 3; }
inline VOID SX1509SetKeypadRows (HSX1509 h1509, UI8 nValue)
   { 
      SX1509SetKeyConfig2(
         h1509, 
         (SX1509GetKeyConfig2(h1509) & 0xC7) | ((nValue & 0x07) << 3)
      ); 
   }
// dimension bits (rows + columns)
inline HSX1509_KEYPAD_DIM SX1509GetKeypadDim (HSX1509 h1509)
   {
      UI8 nConfig = SX1509GetKeyConfig2(h1509);
      return (HSX1509_KEYPAD_DIM) { 
         .nRows = (nConfig & 0x38) >> 3,
         .nCols = nConfig & 0x07
      };
   }
inline VOID SX1509SetKeypadDim (HSX1509 h1509, HSX1509_KEYPAD_DIM dim)
   {
      UI8 nConfig = SX1509GetKeyConfig2(h1509);
      SX1509SetKeyConfig2(
         h1509, 
         (nConfig & 0xC0) | ((dim.nRows & 0x07) << 3) | (dim.nCols & 0x07)
      ); 
   }
//===========================================================================
// KEYPAD DATA REGISTER
//===========================================================================
HSX1509_KEYPAD_DATA SX1509GetKeyData (HSX1509 h1509);
inline UI8 SX1509GetKeyData1 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_KEYDATA1); }
inline UI8 SX1509GetKeyData2 (HSX1509 h1509)
   { return SX1509Get8(h1509, SX1509_REG_KEYDATA2); }
#endif // __SX1509_H
