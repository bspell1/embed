//===========================================================================
// Module:  debug.h
// Purpose: AVR debugging framework
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
#ifndef __DEBUG_H
#define __DEBUG_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// DEBUG CONFIGURATION
// DEBUG_TRACE:   debug trace handler (UartTrace, etc)
//===========================================================================
#ifndef DEBUG_TRACE
#  define DebugTrace(psz, ...)
#else
#  define DebugTrace(psz, ...)      DEBUG_TRACE(psz, __VA_ARGS__)
#endif
//===========================================================================
// TRACING
//===========================================================================
#ifndef DEBUG
#  define Trace(psz, ...)
#else
#  define Trace(psz, ...)           DebugTrace(psz, __VA_ARGS__)
#endif
//===========================================================================
// ASSERTIONS
//===========================================================================
#ifndef DEBUG
#  define Assert(b)
#else
#  define Assert(b)                                                        \
      if (!(b))                                                            \
      {                                                                    \
         Trace(                                                            \
            "Assert:%s():%d\n",                                            \
            __FUNCTION__,                                                  \
            (UI16)__LINE__                                                 \
         );                                                                \
         AvrAbort();                                                       \
      }
#endif
#endif // __DEBUG_H
