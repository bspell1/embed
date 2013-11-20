//===========================================================================
// Module:  lab.c
// Purpose: AVR test program
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
#include "lab.h"
#include "uart.h"
#include "i2cmast.h"
#include "mpu6050.h"
#include "tlc5940.h"
#include "pwmbang.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: main >----------------------------------------------
// Purpose:    program entry point
// Parameters: none
// Returns:    0 if successful
//             nonzero otherwise
//---------------------------------------------------------------------------
int main ()
{
   sei();
   PinSetOutput(PIN_D4);
   UartInit(&(UART_CONFIG) { NULL, NULL });
   I2cInit();
   Mpu6050Init();
   Mpu6050Wake();
   //Mpu6050DisableTemp();
   Mpu6050SetClockSource(MPU6050_CLOCK_PLLGYROX);
   for ( ; ; )
   {
      F32 z = Mpu6050ReadAccelZ();
      UartSend(&z, sizeof(z));
      _delay_ms(1000);
      PinToggle(PIN_D4);
   }
   return 0;
}
