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
#include <avr/eeprom.h>
//-------------------[      Project Include Files      ]-------------------//
#include "proto.h"
#include "uart.h"
#include "tlc5940.h"
#include "sx1509.h"
#include "stepmoto.h"
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// PROTOCOL SIGNATURE
//===========================================================================
#define PROTO_SIGNATURE             ((BYTE)0xC0)
//===========================================================================
// DEVICE ADDRESSING
//===========================================================================
// standard address values
#define PROTO_ADDRESS_UNKNOWN       ((BYTE)0xFF)   // received only by unaddressed devices
#define PROTO_ADDRESS_BROADCAST     ((BYTE)0x00)   // received by all devices
// this device's address
static volatile BYTE g_bAddress   = PROTO_ADDRESS_UNKNOWN;
static BYTE EEMEM    g_bEEAddress = PROTO_ADDRESS_UNKNOWN;
//===========================================================================
// PROTOCOL COMMANDS
//===========================================================================
// protocol command map
#define PROTO_COMMAND_PING          (0x00)
#define PROTO_COMMAND_SETADDRESS    (0x01)
#define PROTO_COMMAND_SETPWM        (0x02)
#define PROTO_COMMAND_STEPMOTOR     (0x03)
#define PROTO_COMMAND_STOPMOTOR     (0x04)
#define PROTO_COMMAND_MAX           (sizeof(g_pParamMap) / sizeof(*g_pParamMap) - 1)
// protocol command dispatchers
static VOID    DispatchPing         ();
static VOID    DispatchSetAddress   ();
static VOID    DispatchSetPwm       ();
static VOID    DispatchStepMotor    ();
static VOID    DispatchStopMotor    ();
// command dispatch mapping
static const struct
{
   UI8   nParams;                   // parameter count
   VOID  (*pfnDispatch)();          // dispatch function
} g_pParamMap[] = 
{
   { 1, DispatchPing },
   { 1, DispatchSetAddress },
   { 3, DispatchSetPwm },
   { 4, DispatchStepMotor },
   { 1, DispatchStopMotor }
};
//===========================================================================
// PROTOCOL BUFFERS
//===========================================================================
static volatile struct
{
   union
   {
      BYTE     pbBuffer[8];
      struct
      {
         BYTE  bSignature;
         BYTE  bAddress;
         BYTE  bCommand;
         BYTE  bParam0;
         BYTE  bParam1;
         BYTE  bParam2;
         BYTE  bParam3;
         BYTE  bParam4;
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
   g_bAddress = eeprom_read_byte(&g_bEEAddress);
}
//-----------< FUNCTION: ProtoRecvByte >-------------------------------------
// Purpose:    dispatches an incoming protocol byte
// Parameters: bRecv - the byte received
// Returns:    none
//---------------------------------------------------------------------------
VOID ProtoRecvByte (BYTE bRecv)
{
   // disable recursive interrupts but
   // run the protocol interruptible
   RegSetLo(UCSR0B, RXCIE0);
   sei();
   // read and decode the protocol bytes
   g_Receive.pbBuffer[g_Receive.cbBuffer++] = bRecv;
   switch (g_Receive.cbBuffer)
   {
      case 1:     // signature byte
         if (g_Receive.bSignature != PROTO_SIGNATURE)
            g_Receive.cbBuffer = 0;
         break;
      case 2:     // address byte
         if (g_Receive.bAddress != g_bAddress && 
             g_Receive.bAddress != PROTO_ADDRESS_BROADCAST)
         g_Receive.cbBuffer = 0;
         break;
      case 3:     // command byte
         if (g_Receive.bCommand > PROTO_COMMAND_MAX)
         {
            g_Receive.cbBuffer = 0;
            break;
         }
      default:    // parameter byte
         // dispatch when all parameters have been received
         if (g_Receive.cbBuffer - 3 == g_pParamMap[g_Receive.bCommand].nParams)
         {
            g_pParamMap[g_Receive.bCommand].pfnDispatch();
            g_Receive.cbBuffer = 0;
         }
         break;
   }
   // re-enable UART interrupts
   RegSetHi(UCSR0B, RXCIE0);
}
//-----------< FUNCTION: DispatchPing >--------------------------------------
// Purpose:    executes a ping command
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchPing ()
{
   UartSendByte(g_Receive.bParam0);
}
//-----------< FUNCTION: DispatchSetAddress >--------------------------------
// Purpose:    executes an address assignment command
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchSetAddress ()
{
   g_bAddress = g_Receive.bParam0;
   eeprom_write_byte(&g_bEEAddress, g_bAddress);
}
//-----------< FUNCTION: DispatchSetPwm >------------------------------------
// Purpose:    sets a PWM duty cycle value on the TLC5940
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
static VOID DispatchSetPwm ()
{
   // decode the PWM channel
   // decode the duty cyle
   UI8  nChannel = g_Receive.bParam0;
   UI16 nDuty    = ((UI16)g_Receive.bParam1 << 8) | g_Receive.bParam2;
   // update the PWM driver
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
