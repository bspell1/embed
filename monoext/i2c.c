//===========================================================================
// Module:  i2c.c
// Purpose: mono I2C serial bus interop extensions
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
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
//-------------------[      Project Include Files      ]-------------------//
#include "monoext.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: I2cSetSlave >---------------------------------------
// Purpose:    sets the slave address for an I2C transaction
// Parameters: fd   - I2C device file descriptor
//             addr - slave address (7-bit or 10-bit big endian)
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "I2cSetSlave",
//       SetLastError = true )]
//    private static extern Int32 SetSlave (
//       Int32 fd, 
//       Int32 addr
//    );
//---------------------------------------------------------------------------
int I2cSetSlave (int fd, int addr)
{
   return ioctl(fd, I2C_SLAVE, addr);
}
//-----------< FUNCTION: I2cSendReceive >--------------------------------------
// Purpose:    performs a combination read/write I2C transaction
// Parameters: fd         - I2C device file descriptor
//             addr       - I2C slave address, required for ioctl
//             pTxBuffer  - transmit buffer
//             cbTxOffset - offset into transmit buffer
//             cbTxLength - number of bytes to transmit
//             pRxBuffer  - receive buffer
//             cbRxOffset - offset into receive buffer
//             cbRxLength - number of bytes to receive
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "I2cSendReceive",
//       SetLastError = true )]
//    private static extern Int32 SendReceive (
//       Int32  fd, 
//       Int32  addr,
//       IntPtr pTxBuffer, 
//       Int32  cbTxOffset,
//       Int32  cbTxLength,
//       IntPtr pRxBuffer, 
//       Int32  cbRxOffset,
//       Int32  cbRxLength
//    );
//---------------------------------------------------------------------------
int I2cSendReceive (
   int     fd, 
   int     addr,
   void*   pTxBuffer, 
   int32_t cbTxOffset,
   int32_t cbTxLength,
   void*   pRxBuffer, 
   int32_t cbRxOffset,
   int32_t cbRxLength)
{
   return ioctl(
      fd, 
      I2C_RDWR, 
      &(struct i2c_rdwr_ioctl_data)
      {
         .nmsgs = 2,
         .msgs  = (struct i2c_msg[])
         {
            {
               .addr  = addr,
               .flags = 0,
               .len   = cbTxLength,
               .buf   = (uint8_t*)pTxBuffer + cbTxOffset
            },
            {
               .addr  = addr,
               .flags = I2C_M_RD,
               .len   = cbRxLength,
               .buf   = (uint8_t*)pRxBuffer + cbRxOffset
            }
         }
      }
   );
}
