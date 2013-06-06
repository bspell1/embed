//===========================================================================
// Module:  eventfd.c
// Purpose: eventfd system calls
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
#include <sys/eventfd.h>
//-------------------[      Project Include Files      ]-------------------//
#include "monoext.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: EventFDCreate >-------------------------------------
// Purpose:    creates a new eventfd handle
// Parameters: value - the initial eventfd value
//             flags - eventfd flags
// Returns:    the new event file descriptor if successful
//             -1 otherwise
// Usage:      
//    [DllImport("monoext", EntryPoint = "EventFDCreate", SetLastError = true)]
//    public static extern Int32 EventFDCreate (
//       Int32 value,
//       Int32 flags
//    );
//---------------------------------------------------------------------------
int EventFDCreate (int value, int flags)
{
   return eventfd(value, flags);
}
