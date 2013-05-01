//===========================================================================
// Module:  i2cmast.h
// Purpose: AVR I2C master driver
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
#ifndef __I2CMAST_H
#define __I2CMAST_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// I2C INTERFACE
//===========================================================================
// send/receive completion callback
typedef  VOID (*I2C_CALLBACK) (BOOL bXferOk, UI8 nSlaveAddr, BSIZE cbMessage);
// I2C API
VOID     I2cInit              (I2C_CALLBACK pCallback);
BOOL     I2cIsBusy            ();
VOID     I2cSend              (UI8   nSlaveAddr, 
                               PVOID pvMessage, 
                               BSIZE cbMessage);
VOID     I2cReceive           (UI8   nSlaveAddr, 
                               PVOID pvMessage, 
                               BSIZE cbBuffer);
#endif // __I2CMAST_H
