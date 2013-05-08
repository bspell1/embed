//===========================================================================
// Module:  proto.c
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
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
#include <avr/interrupt.h>
//-------------------[      Project Include Files      ]-------------------//
#include "proto.h"
#include "uart.h"
#include "tlc5940.h"
#include "sx1509.h"
#include "stepmoto.h"
//-------------------[       Module Definitions        ]-------------------//
#define PROTO_COMMAND_NONE          (0x00)
#define PROTO_COMMAND_SETOUTPUT     (0x01)
#define PROTO_COMMAND_SETSERVO      (0x02)
#define PROTO_COMMAND_STEPMOTOR     (0x03)
#define PROTO_COMMAND_STOPMOTOR     (0x04)
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: ProtoInit >-----------------------------------------
// Purpose:    initializes the protocol module
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID ProtoInit ()
{
}
//-----------< INTERRUPT: USART_RX_vect >------------------------------------
// Purpose:    responds to UART receive interrupts
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
ISR(USART_RX_vect)
{
   BYTE nCommand = UartRecv();                        // read the next command byte
   switch (nCommand)
   {
      case PROTO_COMMAND_SETOUTPUT: {                 // set a digital output pin
         UI8 nChannel = UartRecv();                   // . read the output channel (0-64)
         BIT bValue   = UartRecv() & 0x01;            // . decode the output value
         sei();                                       // . enable interrupts for I2C read/write
         SX1509SetDataBit(                            // . write the output data
            nChannel / 16, 
            nChannel % 16,
            bValue
         );
      }  break;
      case PROTO_COMMAND_SETSERVO: {                  // set a servo duty cycle value
         UI8  nChannel = UartRecv();                  // . read the servo channel byte
         BYTE nDutyHi  = UartRecv();                  // . read the duty cycle word
         BYTE nDutyLo  = UartRecv();
         UI16 nDuty = ((UI16)nDutyHi << 8) | nDutyLo; // . encode the duty cyle word
         Tlc5940SetDuty(                              // . update the servo driver
            nChannel / 16, 
            nChannel % 16, 
            nDuty
         );             
      }  break;
      case PROTO_COMMAND_STEPMOTOR: {                 // run a step motor
         UI8 nMotor = UartRecv();                     // . read the motor number byte
         UI8 nDelay = UartRecv();                     // . read the stage delay byte
         I8  nSteps = (I8)UartRecv();                 // . read the number of steps
         StepMotorRun(nMotor, nDelay, nSteps);        // . run the motor
      }  break;
      case PROTO_COMMAND_STOPMOTOR: {                 // stop a step motor
         UI8 nMotor = UartRecv();                     // . read the motor number byte
         StepMotorStop(nMotor);                       // . stop the motor
      }  break;
      default:                                        // unknown command
         break;
   }
}
