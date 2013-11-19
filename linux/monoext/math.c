//===========================================================================
// Module:  math.c
// Purpose: mono math extensions
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
#include <stdio.h>
#include <math.h>
//-------------------[      Project Include Files      ]-------------------//
#include "monoext.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: MonoMathAtan2 >-------------------------------------
// Purpose:    atan2 wrapper for mono
//             workaround for broken mono math libraries
// Parameters: y - y axis value
//             x - x axis value
// Returns:    atan2 result
// Usage:      
//    [DllImport("monoext", EntryPoint = "MonoMathAtan2F")]
//    private static extern void Atan2 (ref float y, ref float x, out float result);
//    [DllImport("monoext", EntryPoint = "MonoMathAtan2D")]
//    private static extern void Atan2 (ref double y, ref double x, out double result);
//---------------------------------------------------------------------------
void MonoMathAtan2F (float* py, float* px, float* pResult)
{
   *pResult = atan2f(*py, *px);
}
void MonoMathAtan2D (double* py, double* px, float* pResult)
{
   *pResult = atan2(*py, *px);
}
