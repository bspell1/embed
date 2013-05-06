//===========================================================================
// Module:  proto.h
// Purpose: locomoto driver communication protocol
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
#ifndef __PROTO_H
#define __PROTO_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// PROTOCOL CONFIGURATION
// . PROTO_BAUD: the UART baud rate for the protocol interface
//===========================================================================
#ifndef PROTO_BAUD
#  define PROTO_BAUD                (57600)           // base baud rate
#  define PROTO_BAUD_2X             (TRUE)            // 2x multiplier (115200 baud)
#endif
//===========================================================================
// PROTOCOL API
//===========================================================================
VOID  ProtoInit   ();
#endif // __PROTO_H
