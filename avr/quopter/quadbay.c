//===========================================================================
// Module:  quadbay.c
// Purpose: quadcopter bomb bay controller
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
#include "quadbay.h"
//-------------------[       Module Definitions        ]-------------------//
#define PWM_MIN         250                     // 1ms duty for 90deg CCW
#define PWM_MAX         500                     // 2ms duty for 90deg CW
#define PWMA_CLOSE      PWM_MIN
#define PWMB_CLOSE      PWM_MAX
#define PWMA_OPEN       PWM_MAX
#define PWMB_OPEN       PWM_MIN
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: QuadBayInit >---------------------------------------
// Purpose:    module initialization
// Parameters: pConfig - module configuration
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadBayInit (PQUADBAY_CONFIG pConfig)
{
   IgnoreParam(pConfig);
   // timer1 configuration
   TCCR1A |= (1 << COM1A1) | (1 << COM1B1);     // CCR on OC1A/OC1B
   TCCR1A |= (1 << WGM11) | (0 << WGM10);       // FPWM, freq=ICR1, duty=OCR1A/OCR1B, output OC1A/OC1B
   TCCR1B |= (1 << WGM13) | (1 << WGM12);       //
   TCCR1B |= AvrClk1Scale(64);                  // prescaler of 64, base freq=250kHz
   ICR1    = AvrClkTop(64, 50);                 // set frequency to 50Hz
   OCR1A   = PWMA_CLOSE;                        // set initial state to closed
   OCR1B   = PWMB_CLOSE;                        //
   PinSetOutput(PIN_OC1A);                      // enable OC1A/OC1B pin outputs
   PinSetOutput(PIN_OC1B);                      //
}
//-----------< FUNCTION: QuadBayIsOpen >-------------------------------------
// Purpose:    determines the bomb bay status
// Parameters: none
// Returns:    true if the bomb bay is open
//             false otherwise
//---------------------------------------------------------------------------
BOOL QuadBayIsOpen ()
{
   return OCR1A == PWMA_OPEN;
}
//-----------< FUNCTION: QuadBayControl >------------------------------------
// Purpose:    controls the bomb bay
// Parameters: bOpen - true to open the bay, false to close it
// Returns:    none
//---------------------------------------------------------------------------
VOID QuadBayControl (BOOL bOpen)
{
   if (bOpen)
   {
      OCR1A = PWMA_OPEN;
      OCR1B = PWMB_OPEN;
   }
   else
   {
      OCR1A = PWMA_CLOSE;
      OCR1B = PWMB_CLOSE;
   }
}
