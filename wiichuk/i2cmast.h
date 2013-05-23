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
// I2C CONFIGURATION
// . I2C_FREQUENCY            I2C frequency, in Hz
// . I2C_BUFFER_SIZE          size of the I2C send/receive buffer, in bytes
//===========================================================================
#ifndef I2C_FREQUENCY
#  define I2C_FREQUENCY       400000
#endif
#ifndef I2C_BUFFER_SIZE
#  define I2C_BUFFER_SIZE     16
#endif
//===========================================================================
// I2C INTERFACE
//===========================================================================
// I2C API
VOID     I2cInit        ();
BOOL     I2cIsBusy      ();
VOID     I2cWait        ();
UI8      I2cSendRecv    (UI8    nSlaveAddr, 
                         PCVOID pvSend, 
                         BSIZE  cbSend,
                         PVOID  pvRecv, 
                         BSIZE  cbRecv);
// I2C one-way helpers
inline VOID I2cSend (UI8 nSlaveAddr, PCVOID pvSend, BSIZE cbSend)
   { I2cSendRecv(nSlaveAddr, pvSend, cbSend, NULL, 0); }
inline UI8 I2cRecv (UI8 nSlaveAddr, PVOID pvRecv, BSIZE cbRecv)
   { return I2cSendRecv(nSlaveAddr, NULL, 0, pvRecv, cbRecv); }
#endif // __I2CMAST_H
