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
// SX1509 DATA STRUCTURES
//===========================================================================
// row/column configuration
typedef struct tagSX1509KeypadDim
{
   UI8   nRows;
   UI8   nCols;
} SX1509_KEYPAD_DIM;
// row/column data
typedef struct tagSX1509KeypadData
{
   UI8   nRow;
   UI8   nCol;
} SX1509_KEYPAD_DATA;
// PWM configuration
typedef struct tagSX1509PWM_CONFIG
{
   UI8   nOnTime;
   UI8   nOnIntensity;
   UI8   nOffTime;
   UI8   nOffIntensity;
   UI8   nFadeInTime;
   UI8   nFadeOutTime;
} SX1509_PWM_CONFIG;
//===========================================================================
// SX1509 I/O PINS
//===========================================================================
#define SX1509_IO_0                       0x00
#define SX1509_IO_1                       0x01
#define SX1509_IO_2                       0x02
#define SX1509_IO_3                       0x03
#define SX1509_IO_4                       0x04
#define SX1509_IO_5                       0x05
#define SX1509_IO_6                       0x06
#define SX1509_IO_7                       0x07
#define SX1509_IO_8                       0x08
#define SX1509_IO_9                       0x09
#define SX1509_IO_10                      0x0A
#define SX1509_IO_11                      0x0B
#define SX1509_IO_12                      0x0C
#define SX1509_IO_13                      0x0D
#define SX1509_IO_14                      0x0E
#define SX1509_IO_15                      0x0F
//===========================================================================
// SX1509 REGISTERS
//===========================================================================
// for initialization
#define SX1509_REG_INVALID                0xFF
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
// PWM TIME REGISTER VALUES
//===========================================================================
#define SX1509_ONTIME_INFINITE            0x00
#define SX1509_OFFTIME_INFINITE           0x00
#define SX1509_FADEIN_OFF                 0x00
#define SX1509_FADEOUT_OFF                0x00
//===========================================================================
// HIGH INPUT REGISTER VALUES
//===========================================================================
#define SX1509_HIGHINPUT_OFF              BIT_LO
#define SX1509_HIGHINPUT_ON               BIT_HI
//===========================================================================
// SX1509 CORE API
//===========================================================================
// API initialiization (addresses include 0x3E, 0x3F, 0x70, 0x71)
VOID     SX1509Init        ();
// register read/write
UI8      SX1509Get8        (UI8 nModule, UI8 nReg);
VOID     SX1509Set8        (UI8 nModule, UI8 nReg, UI8 nValue);
UI16     SX1509Get16       (UI8 nModule, UI8 nReg);
VOID     SX1509Set16       (UI8 nModule, UI8 nReg, UI16 nValue);
UI32     SX1509Get32       (UI8 nModule, UI8 nReg);
VOID     SX1509Set32       (UI8 nModule, UI8 nReg, UI32 nValue);
//===========================================================================
// REGISTER BIT MANIPULATION
//===========================================================================
inline BIT SX1509GetBit (UI8 nModule, UI8 nReg, UI8 nBit)
   { return BitTest(SX1509Get8(nModule, nReg), nBit); }
inline VOID SX1509SetHi (UI8 nModule, UI8 nReg, UI8 nBit)
   { SX1509Set8(nModule, nReg, SX1509Get8(nModule, nReg) | BitMask(nBit)); }
inline VOID SX1509SetLo (UI8 nModule, UI8 nReg, UI8 nBit)
   { SX1509Set8(nModule, nReg, SX1509Get8(nModule, nReg) & BitUnmask(nBit)); }
inline VOID SX1509SetBit (UI8 nModule, UI8 nReg, UI8 nBit, BOOL fValue)
   {  if (fValue) 
         SX1509SetHi(nModule, nReg, nBit); 
      else 
         SX1509SetLo(nModule, nReg, nBit); 
   }
inline VOID SX1509Toggle (UI8 nModule, UI8 nReg, UI8 nBit)
   { SX1509Set8(nModule, nReg, SX1509Get8(nModule, nReg) ^ BitMask(nBit)); }
//===========================================================================
// INPUT DISABLE REGISTER
//===========================================================================
inline UI16 SX1509GetInputDisable (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_INPUTDISABLE); }
inline VOID SX1509SetInputDisable (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_INPUTDISABLE, nValue); }
inline UI8 SX1509GetInputDisableA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INPUTDISABLEA); }
inline VOID SX1509SetInputDisableA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INPUTDISABLEA, nValue); }
inline UI8 SX1509GetInputDisableB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INPUTDISABLEB); }
inline VOID SX1509SetInputDisableB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INPUTDISABLEB, nValue); }
inline BIT SX1509GetInputDisableBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_INPUTDISABLEA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
inline VOID SX1509SetInputDisableBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_INPUTDISABLEA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_INPUTDISABLEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInputDisable (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_INPUTDISABLEA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_INPUTDISABLEB, nIo - 8);
   }
//===========================================================================
// LONG SLEW REGISTER
//===========================================================================
inline UI16 SX1509GetLongSlew (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_LONGSLEW); }
inline VOID SX1509SetLongSlew (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_LONGSLEW, nValue); }
inline UI8 SX1509GetLongSlewA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LONGSLEWA); }
inline VOID SX1509SetLongSlewA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LONGSLEWA, nValue); }
inline UI8 SX1509GetLongSlewB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LONGSLEWB); }
inline VOID SX1509SetLongSlewB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LONGSLEWB, nValue); }
inline BIT SX1509GetLongSlewBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_LONGSLEWA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_LONGSLEWB, nIo - 8);
   }
inline VOID SX1509SetLongSlewBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_LONGSLEWA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_LONGSLEWB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleLongSlew (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_LONGSLEWA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_LONGSLEWB, nIo - 8);
   }
//===========================================================================
// LOW DRIVE REGISTER
//===========================================================================
inline UI16 SX1509GetLowDrive (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_LOWDRIVE); }
inline VOID SX1509SetLowDrive (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_LOWDRIVE, nValue); }
inline UI8 SX1509GetLowDriveA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LOWDRIVEA); }
inline VOID SX1509SetLowDriveA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LOWDRIVEA, nValue); }
inline UI8 SX1509GetLowDriveB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LOWDRIVEB); }
inline VOID SX1509SetLowDriveB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LOWDRIVEB, nValue); }
inline BIT SX1509GetLowDriveBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_LOWDRIVEA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
inline VOID SX1509SetLowDriveBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_LOWDRIVEA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_LOWDRIVEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleLowDrive (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_LOWDRIVEA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_LOWDRIVEB, nIo - 8);
   }
//===========================================================================
// PULL-UP REGISTER
//===========================================================================
inline UI16 SX1509GetPullUp (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_PULLUP); }
inline VOID SX1509SetPullUp (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_PULLUP, nValue); }
inline UI8 SX1509GetPullUpA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PULLUPA); }
inline VOID SX1509SetPullUpA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PULLUPA, nValue); }
inline UI8 SX1509GetPullUpB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PULLUPB); }
inline VOID SX1509SetPullUpB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PULLUPB, nValue); }
inline BIT SX1509GetPullUpBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_PULLUPA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_PULLUPA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_PULLUPA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_PULLUPB, nIo - 8);
   }
inline VOID SX1509SetPullUpBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_PULLUPA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_PULLUPB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePullUp (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_PULLUPA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_PULLUPB, nIo - 8);
   }
//===========================================================================
// PULL-DOWN REGISTER
//===========================================================================
inline UI16 SX1509GetPullDown (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_PULLDOWN); }
inline VOID SX1509SetPullDown (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_PULLDOWN, nValue); }
inline UI8 SX1509GetPullDownA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PULLDOWNA); }
inline VOID SX1509SetPullDownA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PULLDOWNA, nValue); }
inline UI8 SX1509GetPullDownB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PULLDOWNB); }
inline VOID SX1509SetPullDownB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PULLDOWNB, nValue); }
inline BIT SX1509GetPullDownIo (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_PULLDOWNA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_PULLDOWNB, nIo - 8);
   }
inline VOID SX1509SetPullDownIo (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_PULLDOWNA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_PULLDOWNB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePullDown (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_PULLDOWNA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_PULLDOWNB, nIo - 8);
   }
//===========================================================================
// OPEN DRAIN REGISTER
//===========================================================================
inline UI16 SX1509GetOpenDrain (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_OPENDRAIN); }
inline VOID SX1509SetOpenDrain (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_OPENDRAIN, nValue); }
inline UI8 SX1509GetOpenDrainA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_OPENDRAINA); }
inline VOID SX1509SetOpenDrainA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_OPENDRAINA, nValue); }
inline UI8 SX1509GetOpenDrainB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_OPENDRAINB); }
inline VOID SX1509SetOpenDrainB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_OPENDRAINB, nValue); }
inline BIT SX1509GetOpenDrainIo (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_OPENDRAINA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_OPENDRAINB, nIo - 8);
   }
inline VOID SX1509SetOpenDrainIo (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_OPENDRAINA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_OPENDRAINB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleOpenDrain (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_OPENDRAINA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_OPENDRAINB, nIo - 8);
   }
//===========================================================================
// POLARITY REGISTER
//===========================================================================
inline UI16 SX1509GetPolarity (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_POLARITY); }
inline VOID SX1509SetPolarity (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_POLARITY, nValue); }
inline UI8 SX1509GetPolarityA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_POLARITYA); }
inline VOID SX1509SetPolarityA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_POLARITYA, nValue); }
inline UI8 SX1509GetPolarityB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_POLARITYB); }
inline VOID SX1509SetPolarityB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_POLARITYB, nValue); }
inline BIT SX1509GetPolarityBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_POLARITYA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_POLARITYA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_POLARITYA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_POLARITYB, nIo - 8);
   }
inline VOID SX1509SetPolarityBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_POLARITYA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_POLARITYB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePolarity (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_POLARITYA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_POLARITYB, nIo - 8);
   }
//===========================================================================
// DIRECTION REGISTER
//===========================================================================
inline UI16 SX1509GetDir (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_DIR); }
inline VOID SX1509SetDir (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_DIR, nValue); }
inline UI8 SX1509GetDirA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DIRA); }
inline VOID SX1509SetDirA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DIRA, nValue); }
inline UI8 SX1509GetDirB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DIRB); }
inline VOID SX1509SetDirB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DIRB, nValue); }
inline BIT SX1509GetDirBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_DIRA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_DIRA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_DIRA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_DIRA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_DIRB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleDir (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_DIRA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_DIRB, nIo - 8);
   }
inline VOID SX1509SetDirOutput (UI8 nModule, UI8 nIo)
   { SX1509SetDirBit(nModule, nIo, SX1509_DIR_OUTPUT); }
inline VOID SX1509SetDirInput (UI8 nModule, UI8 nIo)
   { SX1509SetDirBit(nModule, nIo, SX1509_DIR_INPUT); }
//===========================================================================
// DATA REGISTER
//===========================================================================
inline UI16 SX1509GetData (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_DATA); }
inline VOID SX1509SetData (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_DATA, nValue); }
inline UI8 SX1509GetDataA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DATAA); }
inline VOID SX1509SetDataA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DATAA, nValue); }
inline UI8 SX1509GetDataB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DATAB); }
inline VOID SX1509SetDataB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DATAB, nValue); }
inline BIT SX1509GetDataBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_DATAA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_DATAA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_DATAA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_DATAB, nIo - 8);
   }
inline VOID SX1509SetDataBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_DATAA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_DATAB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleData (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_DATAA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_DATAB, nIo - 8);
   }
//===========================================================================
// INTERRUPT MASK REGISTER
//===========================================================================
inline UI16 SX1509GetInterruptMask (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_INTERRUPTMASK); }
inline VOID SX1509SetInterruptMask (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_INTERRUPTMASK, nValue); }
inline UI8 SX1509GetInterruptMaskA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INTERRUPTMASKA); }
inline VOID SX1509SetInterruptMaskA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INTERRUPTMASKA, nValue); }
inline UI8 SX1509GetInterruptMaskB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INTERRUPTMASKB); }
inline VOID SX1509SetInterruptMaskB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INTERRUPTMASKB, nValue); }
inline BIT SX1509GetInterruptMaskBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_INTERRUPTMASKA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
inline VOID SX1509SetInterruptMaskBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_INTERRUPTMASKA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_INTERRUPTMASKB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInterruptMask (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_INTERRUPTMASKA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_INTERRUPTMASKB, nIo - 8);
   }
//===========================================================================
// SENSE REGISTER
//===========================================================================
inline UI32 SX1509GetSense (UI8 nModule)
   { return SX1509Get32(nModule, SX1509_REG_SENSE); }
inline VOID SX1509SetSense (UI8 nModule, UI32 nValue)
   { SX1509Set32(nModule, SX1509_REG_SENSE, nValue); }
inline UI8 SX1509GetSenseLowA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_SENSELOWA); }
inline VOID SX1509SetSenseLowA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_SENSELOWA, nValue); }
inline UI8 SX1509GetSenseHighA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_SENSEHIGHA); }
inline VOID SX1509SetSenseHighA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_SENSEHIGHA, nValue); }
inline UI8 SX1509GetSenseLowB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_SENSELOWB); }
inline VOID SX1509SetSenseLowB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_SENSELOWB, nValue); }
inline UI8 SX1509GetSenseHighB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_SENSEHIGHB); }
inline VOID SX1509SetSenseHighB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_SENSEHIGHB, nValue); }
inline UI8 SX1509GetSenseMode (UI8 nModule, UI8 nIo)
   {  
      UI8 nShift = (2 * (nIo % 4));
      if (nIo < 4)
         return (SX1509GetSenseLowA(nModule) >> nShift) & 3;
      else if (nIo < 8)
         return (SX1509GetSenseHighA(nModule) >> nShift) & 3;
      else if (nIo < 12)
         return (SX1509GetSenseLowB(nModule) >> nShift) & 3;
      else
         return (SX1509GetSenseHighB(nModule) >> nShift) & 3;
   }
inline VOID SX1509SetSenseMode (UI8 nModule, UI8 nIo, UI8 nMode)
   { 
      UI8 nShift = (2 * (nIo % 4));
      UI8 nMask = 3 << nShift;
      nMode = (nMode << nShift) & nMask;
      if (nIo < 4)
         SX1509SetSenseLowA(nModule, (SX1509GetSenseLowA(nModule) & nMask) | nMode);
      else if (nIo < 8)
         SX1509SetSenseHighA(nModule, (SX1509GetSenseHighA(nModule) & nMask) | nMode);
      else if (nIo < 12)
         SX1509SetSenseLowB(nModule, (SX1509GetSenseLowB(nModule) & nMask) | nMode);
      else
         SX1509SetSenseHighB(nModule, (SX1509GetSenseHighB(nModule) & nMask) | nMode);
   }
inline VOID SX1509SetSenseNone (UI8 nModule, UI8 nIo)
   { SX1509SetSenseMode(nModule, nIo, SX1509_SENSE_MODE_NONE); }
inline VOID SX1509SetSenseRising (UI8 nModule, UI8 nIo)
   { SX1509SetSenseMode(nModule, nIo, SX1509_SENSE_MODE_RISING); }
inline VOID SX1509SetSenseFalling (UI8 nModule, UI8 nIo)
   { SX1509SetSenseMode(nModule, nIo, SX1509_SENSE_MODE_FALLING); }
inline VOID SX1509SetSenseBoth (UI8 nModule, UI8 nIo)
   { SX1509SetSenseMode(nModule, nIo, SX1509_SENSE_MODE_BOTH); }
//===========================================================================
// INTERRUPT SOURCE REGISTER
//===========================================================================
inline UI16 SX1509GetInterruptSource (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_INTERRUPTSOURCE); }
inline VOID SX1509SetInterruptSource (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_INTERRUPTSOURCE, nValue); }
inline UI8 SX1509GetInterruptSourceA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INTERRUPTSOURCEA); }
inline VOID SX1509SetInterruptSourceA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INTERRUPTSOURCEA, nValue); }
inline UI8 SX1509GetInterruptSourceB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_INTERRUPTSOURCEB); }
inline VOID SX1509SetInterruptSourceB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_INTERRUPTSOURCEB, nValue); }
inline BIT SX1509GetInterruptSourceBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_INTERRUPTSOURCEA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
inline VOID SX1509SetInterruptSourceBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_INTERRUPTSOURCEA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_INTERRUPTSOURCEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleInterruptSource (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_INTERRUPTSOURCEA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_INTERRUPTSOURCEB, nIo - 8);
   }
//===========================================================================
// EVENT STATUS REGISTER
//===========================================================================
inline UI16 SX1509GetEventStatus (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_EVENTSTATUS); }
inline VOID SX1509SetEventStatus (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_EVENTSTATUS, nValue); }
inline UI8 SX1509GetEventStatusA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_EVENTSTATUSA); }
inline VOID SX1509SetEventStatusA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_EVENTSTATUSA, nValue); }
inline UI8 SX1509GetEventStatusB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_EVENTSTATUSB); }
inline VOID SX1509SetEventStatusB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_EVENTSTATUSB, nValue); }
inline BIT SX1509GetEventStatusBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_EVENTSTATUSA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
inline VOID SX1509SetEventStatusBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_EVENTSTATUSA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_EVENTSTATUSB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleEventStatus (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_EVENTSTATUSA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_EVENTSTATUSB, nIo - 8);
   }
//===========================================================================
// LEVEL SHIFT REGISTER
//===========================================================================
inline UI16 SX1509GetLevelShift (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_LEVELSHIFTER); }
inline VOID SX1509SetLevelShift (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_LEVELSHIFTER, nValue); }
inline UI8 SX1509GetLevelShift1 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LEVELSHIFTER1); }
inline VOID SX1509SetLevelShift1 (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LEVELSHIFTER1, nValue); }
inline UI8 SX1509GetLevelShift2 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_LEVELSHIFTER2); }
inline VOID SX1509SetLevelShift2 (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_LEVELSHIFTER2, nValue); }
inline UI8 SX1509GetLevelShiftMode (UI8 nModule, UI8 nPair)
   {  
      UI8 nShift = (2 * (nPair % 4));
      if (nPair < 4)
         return (SX1509GetLevelShift2(nModule) >> nShift) & 3;
      else
         return (SX1509GetLevelShift1(nModule) >> nShift) & 3;
   }
inline VOID SX1509SetLevelShiftMode (UI8 nModule, UI8 nPair, UI8 nMode)
   { 
      UI8 nShift = (2 * (nPair % 4));
      UI8 nMask = 3 << nShift;
      nMode = (nMode << nShift) & nMask;
      if (nPair < 4)
         SX1509SetLevelShift2(nModule, (SX1509GetLevelShift1(nModule) & nMask) | nMode);
      else
         SX1509SetLevelShift1(nModule, (SX1509GetLevelShift2(nModule) & nMask) | nMode);
   }
inline VOID SX1509SetLevelShiftOff (UI8 nModule, UI8 nPair)
   { SX1509SetLevelShiftMode(nModule, nPair, SX1509_LEVELSHIFT_MODE_OFF); }
inline VOID SX1509SetLevelShiftA2B (UI8 nModule, UI8 nPair)
   { SX1509SetLevelShiftMode(nModule, nPair, SX1509_LEVELSHIFT_MODE_A2B); }
inline VOID SX1509SetLevelShiftB2A (UI8 nModule, UI8 nPair)
   { SX1509SetLevelShiftMode(nModule, nPair, SX1509_LEVELSHIFT_MODE_B2A); }
//===========================================================================
// CLOCK REGISTER
//===========================================================================
inline UI8 SX1509GetClock (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_CLOCK); }
inline VOID SX1509SetClock (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_CLOCK, nValue); }
// output frequency bits (0-3)
inline UI8 SX1509GetClockFrequency (UI8 nModule)
   { return SX1509GetClock(nModule) & 0x07; }
inline VOID SX1509SetClockFrequency (UI8 nModule, UI8 nFrequency)
   { SX1509SetClock(nModule, (SX1509GetClock(nModule) & 0x07) | nFrequency); }
inline BIT SX1509GetClockGpo (UI8 nModule)
   { return (SX1509GetClockFrequency(nModule) == 0x0F) ? BIT_HI : BIT_LO; }
inline VOID SX1509SetClockGpo (UI8 nModule, BIT fValue)
   { SX1509SetClockFrequency(nModule, fValue ? 0x0F : 0x00); }
inline VOID SX1509SetClockGpoLo (UI8 nModule)
   { SX1509SetClockFrequency(nModule, 0x00); }
inline VOID SX1509SetClockGpoHi (UI8 nModule)
   { SX1509SetClockFrequency(nModule, 0x0F); }
inline VOID SX1509ToggleClockGpo (UI8 nModule)
   { SX1509SetClock(nModule, SX1509GetClock(nModule) ^ 0x7); }
// mode bit (4)
inline BIT SX1509GetClockMode (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_CLOCK, 4); }
inline VOID SX1509SetClockMode (UI8 nModule, BIT fMode)
   { SX1509SetBit(nModule, SX1509_REG_CLOCK, 4, fMode); }
inline VOID SX1509SetClockModeInput (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_CLOCK, 4, SX1509_CLOCK_MODE_INPUT); }
inline VOID SX1509SetClockModeOutput (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_CLOCK, 4, SX1509_CLOCK_MODE_OUTPUT); }
// oscillator frequency bits (5-6)
inline UI8 SX1509GetClockSource (UI8 nModule)
   { return (SX1509GetClock(nModule) >> 5) & 3; }
inline VOID SX1509SetClockSource (UI8 nModule, UI8 nSource)
   { SX1509SetClock(nModule, (SX1509GetClock(nModule) & 0x60) | (nSource << 5)); }
inline VOID SX1509SetClockSourceNone (UI8 nModule)
   { SX1509SetClockSource(nModule, SX1509_CLOCK_SOURCE_NONE); }
inline VOID SX1509SetClockSourceOscIn (UI8 nModule)
   { SX1509SetClockSource(nModule, SX1509_CLOCK_SOURCE_OSCIN); }
inline VOID SX1509SetClockSourceInternal (UI8 nModule)
   { SX1509SetClockSource(nModule, SX1509_CLOCK_SOURCE_INTERNAL); }
//===========================================================================
// MISCELLANEOUS REGISTER
//===========================================================================
inline UI8 SX1509GetMisc (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_MISC); }
inline VOID SX1509SetMisc (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_MISC, nValue); }
// auto-clear interrupt bit (0)
inline BOOL SX1509GetMiscInterruptAutoClear (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_MISC, 0); }
inline VOID SX1509SetMiscInterruptAutoClear (UI8 nModule, BOOL fValue)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 0, fValue); }
// auto-increment address BOOL (1)
inline BOOL SX1509GetMiscAddressAutoIncrement (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_MISC, 1); }
inline VOID SX1509SetMiscAddressAutoIncrement (UI8 nModule, BOOL fValue)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 1, fValue); }
// RESET function bit (2)
inline BIT SX1509GetMiscResetMode (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_MISC, 2); }
inline VOID SX1509SetMiscResetMode (UI8 nModule, BIT fValue)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 2, fValue); }
inline VOID SX1509SetMiscResetHard (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 2, SX1509_MISC_RESET_HARD); }
inline VOID SX1509SetMiscResetSoft (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 2, SX1509_MISC_RESET_SOFT); }
// fader mode bit for bank A (3)
inline BIT SX1509GetMiscFaderAMode (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_MISC, 3); }
inline VOID SX1509SetMiscFaderAMode (UI8 nModule, BIT fValue)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 3, fValue); }
inline VOID SX1509SetMiscFaderALinear (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 3, SX1509_MISC_FADER_LINEAR); }
inline VOID SX1509SetMiscFaderALogarithmic (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 3, SX1509_MISC_FADER_LOGARITHMIC); }
// PWM frequency bits (4-6)
inline UI8 SX1509GetMiscPwmFrequency (UI8 nModule)
   { return (SX1509GetMisc(nModule) >> 4) & 7; }
inline VOID SX1509SetMiscPwmFrequency (UI8 nModule, UI8 nFreq)
   { SX1509SetMisc(nModule, (SX1509GetMisc(nModule) & 0x70) | (nFreq << 4)); }
inline VOID SX1509SetMiscPwmOff (UI8 nModule)
   { SX1509SetMiscPwmFrequency(nModule, 0); }
// fader mode bit for bank B (7)
inline BIT SX1509GetMiscFaderBMode (UI8 nModule)
   { return SX1509GetBit(nModule, SX1509_REG_MISC, 7); }
inline VOID SX1509SetMiscFaderBMode (UI8 nModule, BIT fValue)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 7, fValue); }
inline VOID SX1509SetMiscFaderBLinear (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 7, SX1509_MISC_FADER_LINEAR); }
inline VOID SX1509SetMiscFaderBLogarithmic (UI8 nModule)
   { SX1509SetBit(nModule, SX1509_REG_MISC, 7, SX1509_MISC_FADER_LOGARITHMIC); }
//===========================================================================
// PWM DRIVER ENABLE REGISTER
//===========================================================================
inline UI16 SX1509GetPwmEnable (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_PWMDRIVERENABLE); }
inline VOID SX1509SetPwmEnable (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_PWMDRIVERENABLE, nValue); }
inline UI8 SX1509GetPwmEnableA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PWMDRIVERENABLEA); }
inline VOID SX1509SetPwmEnableA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PWMDRIVERENABLEA, nValue); }
inline UI8 SX1509GetPwmEnableB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_PWMDRIVERENABLEB); }
inline VOID SX1509SetPwmEnableB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_PWMDRIVERENABLEB, nValue); }
inline BIT SX1509GetPwmEnableBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_PWMDRIVERENABLEA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
inline VOID SX1509SetPwmEnableBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_PWMDRIVERENABLEA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_PWMDRIVERENABLEB, nIo - 8, fValue);
   }
inline VOID SX1509TogglePwmEnable (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_PWMDRIVERENABLEA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_PWMDRIVERENABLEB, nIo - 8);
   }
//===========================================================================
// DEBOUNCE CONFIGURATION REGISTER
//===========================================================================
inline UI8 SX1509GetDebounceConfig (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DEBOUNCECONFIG); }
inline VOID SX1509SetDebounceConfig (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DEBOUNCECONFIG, nValue); }
// debounce time bits (0-2)   
inline UI8 SX1509GetDebounceTime (UI8 nModule)
   { return SX1509GetDebounceConfig(nModule) & 0x07; }
inline VOID SX1509SetDebounceTime (UI8 nModule, UI8 nValue)
   { 
      SX1509SetDebounceConfig(
         nModule, 
         (SX1509GetDebounceConfig(nModule) & 0xF8) | (nValue & 0x07)
      ); 
   }
//===========================================================================
// DEBOUNCE ENABLE REGISTER
//===========================================================================
inline UI16 SX1509GetDebounceEnable (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_DEBOUNCEENABLE); }
inline VOID SX1509SetDebounceEnable (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_DEBOUNCEENABLE, nValue); }
inline UI8 SX1509GetDebounceEnableA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DEBOUNCEENABLEA); }
inline VOID SX1509SetDebounceEnableA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DEBOUNCEENABLEA, nValue); }
inline UI8 SX1509GetDebounceEnableB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_DEBOUNCEENABLEB); }
inline VOID SX1509SetDebounceEnableB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_DEBOUNCEENABLEB, nValue); }
inline BIT SX1509GetDebounceEnableBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_DEBOUNCEENABLEA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
inline VOID SX1509SetDebounceEnableBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_DEBOUNCEENABLEA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_DEBOUNCEENABLEB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleDebounceEnable (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_DEBOUNCEENABLEA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_DEBOUNCEENABLEB, nIo - 8);
   }
//===========================================================================
// KEYPAD CONFIGURATION REGISTER 1
//===========================================================================
inline UI8 SX1509GetKeyConfig1 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_KEYCONFIG1); }
inline VOID SX1509SetKeyConfig1 (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_KEYCONFIG1, nValue); }
// scan time per row bits (0-2)
inline UI8 SX1509GetScanTime (UI8 nModule)
   { return SX1509GetKeyConfig1(nModule) & 0x07; }
inline VOID SX1509SetScanTime (UI8 nModule, UI8 nValue)
   { 
      SX1509SetKeyConfig1(
         nModule, 
         (SX1509GetKeyConfig1(nModule) & 0xF8) | (nValue & 0x07)
      ); 
   }
// auto sleep time bits (4-6)
inline UI8 SX1509GetScanAutoSleepTime (UI8 nModule)
   { return (SX1509GetKeyConfig1(nModule) & 0x70) >> 4; }
inline VOID SX1509SetScanAutoSleepTime (UI8 nModule, UI8 nValue)
   { 
      SX1509SetKeyConfig1(
         nModule, 
         (SX1509GetKeyConfig1(nModule) & 0x8F) | ((nValue & 0x07) << 4)
      ); 
   }
//===========================================================================
// KEYPAD CONFIGURATION REGISTER 2
//===========================================================================
inline UI8 SX1509GetKeyConfig2 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_KEYCONFIG2); }
inline VOID SX1509SetKeyConfig2 (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_KEYCONFIG2, nValue); }
// column count bits (0-2)
inline UI8 SX1509GetKeypadCols (UI8 nModule)
   { return SX1509GetKeyConfig2(nModule) & 0x07; }
inline VOID SX1509SetKeypadCols (UI8 nModule, UI8 nValue)
   { 
      SX1509SetKeyConfig2(
         nModule, 
         (SX1509GetKeyConfig2(nModule) & 0xF8) | (nValue & 0x07)
      ); 
   }
// row count bits (3-5)
inline UI8 SX1509GetKeypadRows (UI8 nModule)
   { return (SX1509GetKeyConfig2(nModule) & 0x38) >> 3; }
inline VOID SX1509SetKeypadRows (UI8 nModule, UI8 nValue)
   { 
      SX1509SetKeyConfig2(
         nModule, 
         (SX1509GetKeyConfig2(nModule) & 0xC7) | ((nValue & 0x07) << 3)
      ); 
   }
// dimension bits (rows + columns)
inline SX1509_KEYPAD_DIM SX1509GetKeypadDim (UI8 nModule)
   {
      UI8 nConfig = SX1509GetKeyConfig2(nModule);
      return (SX1509_KEYPAD_DIM) { 
         .nRows = (nConfig & 0x38) >> 3,
         .nCols = nConfig & 0x07
      };
   }
inline VOID SX1509SetKeypadDim (UI8 nModule, SX1509_KEYPAD_DIM dim)
   {
      UI8 nConfig = SX1509GetKeyConfig2(nModule);
      SX1509SetKeyConfig2(
         nModule, 
         (nConfig & 0xC0) | ((dim.nRows & 0x07) << 3) | (dim.nCols & 0x07)
      ); 
   }
//===========================================================================
// KEYPAD DATA REGISTER
//===========================================================================
inline UI8 SX1509GetKeyData1 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_KEYDATA1); }
inline UI8 SX1509GetKeyData2 (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_KEYDATA2); }
SX1509_KEYPAD_DATA SX1509GetKeyData (UI8 nModule);
//===========================================================================
// PWM ON TIME REGISTERS
//===========================================================================
UI8 SX1509GetOnTime (UI8 nModule, UI8 nIo);
VOID SX1509SetOnTime (UI8 nModule, UI8 nIo, UI8 nValue);
inline VOID SX1509SetOnInfinite (UI8 nModule, UI8 nIo)
   { SX1509SetOnTime(nModule, nIo, SX1509_ONTIME_INFINITE); }
//===========================================================================
// PWM ON INTENSITY REGISTERS
//===========================================================================
UI8 SX1509GetOnIntensity (UI8 nModule, UI8 nIo);
VOID SX1509SetOnIntensity (UI8 nModule, UI8 nIo, UI8 nValue);
//===========================================================================
// PWM OFF REGISTERS
//===========================================================================
UI8 SX1509GetOff (UI8 nModule, UI8 nIo);
VOID SX1509SetOff (UI8 nModule, UI8 nIo, UI8 nValue);
inline UI8 SX1509GetOffTime (UI8 nModule, UI8 nIo)
   { return SX1509GetOff(nModule, nIo) >> 3; }
inline VOID SX1509SetOffTime (UI8 nModule, UI8 nIo, UI8 nValue)
   { 
      SX1509SetOff(
         nModule, 
         nIo, 
         (SX1509GetOff(nModule, nIo) & 0x07) | ((nValue & 0x1F) << 3)
      ); 
   }
inline VOID SX1509SetOffInfinite (UI8 nModule, UI8 nIo)
   { SX1509SetOffTime(nModule, nIo, SX1509_OFFTIME_INFINITE); }
inline UI8 SX1509GetOffIntensity (UI8 nModule, UI8 nIo)
   { return SX1509GetOff(nModule, nIo) & 0x07; }
inline VOID SX1509SetOffIntensity (UI8 nModule, UI8 nIo, UI8 nValue)
   { 
      SX1509SetOff(
         nModule, 
         nIo, 
         (SX1509GetOff(nModule, nIo) & 0xF8) | (nValue & 0x07)
      ); 
   }
//===========================================================================
// PWM FADE IN TIME REGISTERS
//===========================================================================
UI8 SX1509GetFadeInTime (UI8 nModule, UI8 nIo);
VOID SX1509SetFadeInTime (UI8 nModule, UI8 nIo, UI8 nValue);
inline VOID SX1509SetFadeInOff (UI8 nModule, UI8 nIo)
   { SX1509SetFadeInTime(nModule, nIo, SX1509_FADEIN_OFF); }
//===========================================================================
// PWM FADE OUT TIME REGISTERS
//===========================================================================
UI8 SX1509GetFadeOutTime (UI8 nModule, UI8 nIo);
VOID SX1509SetFadeOutTime (UI8 nModule, UI8 nIo, UI8 nValue);
inline VOID SX1509SetFadeOutOff (UI8 nModule, UI8 nIo)
   { SX1509SetFadeOutTime(nModule, nIo, SX1509_FADEOUT_OFF); }
//===========================================================================
// PWM CONFIGURATION REGISTERS
//===========================================================================
SX1509_PWM_CONFIG SX1509GetPwmConfig (UI8 nModule, UI8 nIo);
VOID SX1509SetPwmConfig (UI8 nModule, UI8 nIo, SX1509_PWM_CONFIG config);
//===========================================================================
// HIGH INPUT (5V vs. 3V3) REGISTER
//===========================================================================
inline UI16 SX1509GetHighInput (UI8 nModule)
   { return SX1509Get16(nModule, SX1509_REG_HIGHINPUT); }
inline VOID SX1509SetHighInput (UI8 nModule, UI16 nValue)
   { SX1509Set16(nModule, SX1509_REG_HIGHINPUT, nValue); }
inline UI8 SX1509GetHighInputA (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_HIGHINPUTA); }
inline VOID SX1509SetHighInputA (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_HIGHINPUTA, nValue); }
inline UI8 SX1509GetHighInputB (UI8 nModule)
   { return SX1509Get8(nModule, SX1509_REG_HIGHINPUTB); }
inline VOID SX1509SetHighInputB (UI8 nModule, UI8 nValue)
   { SX1509Set8(nModule, SX1509_REG_HIGHINPUTB, nValue); }
inline BIT SX1509GetHighInputBit (UI8 nModule, UI8 nIo)
   {  return nIo < 8 ? 
         SX1509GetBit(nModule, SX1509_REG_HIGHINPUTA, nIo) : 
         SX1509GetBit(nModule, SX1509_REG_HIGHINPUTB, nIo - 8);
   }
inline VOID SX1509SetHighInputLo (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetLo(nModule, SX1509_REG_HIGHINPUTA, nIo);
      else
         SX1509SetLo(nModule, SX1509_REG_HIGHINPUTB, nIo - 8);
   }
inline VOID SX1509SetHighInputHi (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509SetHi(nModule, SX1509_REG_HIGHINPUTA, nIo);
      else
         SX1509SetHi(nModule, SX1509_REG_HIGHINPUTB, nIo - 8);
   }
inline VOID SX1509SetHighInputBit (UI8 nModule, UI8 nIo, BIT fValue)
   {  if (nIo < 8)
         SX1509SetBit(nModule, SX1509_REG_HIGHINPUTA, nIo, fValue);
      else
         SX1509SetBit(nModule, SX1509_REG_HIGHINPUTB, nIo - 8, fValue);
   }
inline VOID SX1509ToggleHighInput (UI8 nModule, UI8 nIo)
   {  if (nIo < 8)
         SX1509Toggle(nModule, SX1509_REG_HIGHINPUTA, nIo);
      else
         SX1509Toggle(nModule, SX1509_REG_HIGHINPUTB, nIo - 8);
   }
//===========================================================================
// RESET REGISTER
//===========================================================================
VOID SX1509Reset (UI8 nModule);
#endif // __SX1509_H
