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
// protocol signature byte
#define PROTO_SIGNATURE             (0xC0)
// protocol command map
#define PROTO_COMMAND_PING          (0x00)
#define PROTO_COMMAND_SETOUTPUT     (0x01)
#define PROTO_COMMAND_SETSERVO      (0x02)
#define PROTO_COMMAND_STEPMOTOR     (0x03)
#define PROTO_COMMAND_STOPMOTOR     (0x04)
// protocol command dispatchers
static VOID DispatchPing      ();
static VOID DispatchSetOutput ();
static VOID DispatchSetServo  ();
static VOID DispatchStepMotor ();
static VOID DispatchStopMotor ();
// command dispatch mapping
static struct
{
   UI8   nParams;                   // parameter count
   VOID  (*pfnDispatch)();          // dispatch function
} g_pParamMap[] = 
{
   { 1, DispatchPing },
   { 2, DispatchSetOutput },
   { 3, DispatchSetServo },
   { 4, DispatchStepMotor },
   { 1, DispatchStopMotor }
};
// protocol receive buffer
static struct
{
   union
   {
      BYTE     pbBuffer[6];
      struct
      {
         BYTE  bSignature;
         BYTE  bCommand;
         BYTE  bParam0;
         BYTE  bParam1;
         BYTE  bParam2;
         BYTE  bParam3;
      };
   };
   UI8         cbBuffer;
} g_Receive;
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
   // disable recursive interrupts but
   // run the protocol interruptible
   RegSetLo(UCSR0B, RXCIE0);
   sei();
   // read and decode the protocol bytes
   g_Receive.pbBuffer[g_Receive.cbBuffer++] = UartRecv();
   switch (g_Receive.cbBuffer)
   {
      case 1:
         if (g_Receive.bSignature != PROTO_SIGNATURE)
            g_Receive.cbBuffer = 0;
         break;
      case 2:
         if (g_Receive.bCommand >= sizeof(g_pParamMap))
            g_Receive.cbBuffer = 0;
      default:
         // dispatch when all parameters have been received
         if (g_Receive.cbBuffer - 2 == g_pParamMap[g_Receive.bCommand].nParams)
         {
            g_pParamMap[g_Receive.bCommand].pfnDispatch();
            g_Receive.cbBuffer = 0;
         }
         break;
   }
   // re-enable UART interrupts
   cli();
   RegSetHi(UCSR0B, RXCIE0);
}
//-----------< FUNCTION: DispatchPing >--------------------------------------
// Purpose:    executes a ping command
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchPing ()
{
   UartSend(g_Receive.bParam0);
}
//-----------< FUNCTION: DispatchSetOutput >---------------------------------
// Purpose:    sets an output pin value on the SX1509
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchSetOutput ()
{
   // decode the output channel (0-64)
   // decode the output value bit
   UI8 nChannel = g_Receive.bParam0;
   BIT bValue   = g_Receive.bParam1 & 0x01;
   // enable interrupts for I2C read/write
   // write the output data
   SX1509SetDataBit(nChannel / 16, nChannel % 16, bValue);
}
//-----------< FUNCTION: DispatchSetServo >----------------------------------
// Purpose:    sets a servo duty cycle value on the TLC5940
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchSetServo ()
{
   // decode the servo channel
   // decode the duty cyle
   UI8  nChannel = g_Receive.bParam0;
   UI16 nDuty    = ((UI16)g_Receive.bParam1 << 8) | g_Receive.bParam2;
   // update the servo driver
   Tlc5940SetDuty(nChannel / 16, nChannel % 16, nDuty);             
}
//-----------< FUNCTION: DispatchStepMotor >---------------------------------
// Purpose:    runs a stepper motor a number of steps
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchStepMotor ()
{
   // decode the motor number
   // decode the stage delay
   // decode the step count
   UI8  nMotor = g_Receive.bParam0;
   UI8  nDelay = g_Receive.bParam1;
   I16  nSteps = ((I16)g_Receive.bParam2 << 8) | g_Receive.bParam3;
   // run the motor
   StepMotorRun(nMotor, nDelay, nSteps);
}
//-----------< FUNCTION: DispatchStopMotor >---------------------------------
// Purpose:    stops a stepper motor
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchStopMotor ()
{
   // decode the motor number and stop it
   UI8 nMotor = g_Receive.bParam0;
   StepMotorStop(nMotor);
}
