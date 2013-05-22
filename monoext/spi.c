//===========================================================================
// Module:  spi.c
// Purpose: mono Serial Peripheral Interface (SPI) interop extensions
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
#include <linux/spi/spidev.h>
//-------------------[      Project Include Files      ]-------------------//
#include "monoext.h"
//-------------------[       Module Definitions        ]-------------------//
//-------------------[        Module Variables         ]-------------------//
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SpiGetMode >----------------------------------------
// Purpose:    retrieves the current SPI mode
// Parameters: fd     - SPI device file descriptor
//             pbMode - return SPI mode via here
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiGetMode",
//       SetLastError = true )]
//    private static extern Int32 GetMode (
//       Int32    fd,
//       out Byte pbMode
//    );
//---------------------------------------------------------------------------
int SpiGetMode (int fd, uint8_t* pbMode)
{
   *pbMode = 0;
   return ioctl(fd, SPI_IOC_RD_MODE, pbMode);
}
//-----------< FUNCTION: SpiSetMode >----------------------------------------
// Purpose:    sets the current SPI mode
// Parameters: fd    - SPI device file descriptor
//             bMode - SPI mode to assign
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiSetMode",
//       SetLastError = true )]
//    private static extern Int32 SetMode (
//       Int32 fd,
//       Byte  bMode
//    );
//---------------------------------------------------------------------------
int SpiSetMode (int fd, uint8_t bMode)
{
   return ioctl(fd, SPI_IOC_WR_MODE, &bMode);
}
//-----------< FUNCTION: SpiGetBitsPerWord >---------------------------------
// Purpose:    retrieves the current SPI word size
// Parameters: fd            - SPI device file descriptor
//             pnBitsPerWord - return SPI word size via here
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiGetBitsPerWord",
//       SetLastError = true )]
//    private static extern Int32 GetBitsPerWord (
//       Int32    fd,
//       out Byte pnBitsPerWord
//    );
//---------------------------------------------------------------------------
int SpiGetBitsPerWord (int fd, uint8_t* pnBitsPerWord)
{
   *pnBitsPerWord = 0;
   return ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, pnBitsPerWord);
}
//-----------< FUNCTION: SpiSetBitsPerWord >---------------------------------
// Purpose:    sets the current SPI word size
// Parameters: fd           - SPI device file descriptor
//             nBitsPerWord - SPI word size to assign
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiSetBitsPerWord",
//       SetLastError = true )]
//    private static extern Int32 SetBitsPerWord (
//       Int32 fd,
//       Byte  nBitsPerWord
//    );
//---------------------------------------------------------------------------
int SpiSetBitsPerWord (int fd, uint8_t nBitsPerWord)
{
   return ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &nBitsPerWord);
}
//-----------< FUNCTION: SpiGetClockSpeed >----------------------------------
// Purpose:    retrieves the current SPI clock speed, in Hz
// Parameters: fd            - SPI device file descriptor
//             pnClockSpeed - return SPI word size via here
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiGetClockSpeed",
//       SetLastError = true )]
//    private static extern Int32 GetClockSpeed (
//       Int32     fd,
//       out Int32 pnClockSpeed
//    );
//---------------------------------------------------------------------------
int SpiGetClockSpeed (int fd, uint32_t* pnClockSpeed)
{
   *pnClockSpeed = 0;
   return ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, pnClockSpeed);
}
//-----------< FUNCTION: SpiSetClockSpeed >----------------------------------
// Purpose:    sets the current SPI clock speed, in Hz
// Parameters: fd           - SPI device file descriptor
//             nClockSpeed - SPI word size to assign
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiSetClockSpeed",
//       SetLastError = true )]
//    private static extern Int32 SetClockSpeed (
//       Int32 fd,
//       Int32 nClockSpeed
//    );
//---------------------------------------------------------------------------
int SpiSetClockSpeed (int fd, uint32_t nClockSpeed)
{
   return ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &nClockSpeed);
}
//-----------< FUNCTION: SpiSendReceive >-------------------------------------
// Purpose:    exchanges a byte stream with the SPI device
// Parameters: fd        - SPI device file descriptor
//             tx_buffer - send buffer (null to receive only)
//             tx_offset - send buffer offset
//             rx_buffer - receive buffer (null to send only)
//             rx_offset - receive buffer offset
//             count     - the number of bytes to send/receive
// Returns:    0 if successful
//             -1 otherwise 
// Usage:      
//    [DllImport(
//       "monoext", 
//       EntryPoint = "SpiSendReceive",
//       SetLastError = true )]
//    private static extern Int32 SendReceive (
//       Int32  fd, 
//       IntPtr txBuffer, 
//       Int32  txOffset,
//       IntPtr rxBuffer, 
//       Int32  rxOffset,
//       Int32  count
//    );
//---------------------------------------------------------------------------
int SpiSendReceive (
   int      fd, 
   void*    tx_buffer, 
   int32_t  tx_offset,
   void*    rx_buffer, 
   int32_t  rx_offset,
   int32_t  count)
{
   if (count > 0) {
	   struct spi_ioc_transfer msg; memset(&msg, 0, sizeof(msg));
      msg.tx_buf = (unsigned long)tx_buffer + tx_offset;
      msg.rx_buf = (unsigned long)rx_buffer + rx_offset;
      msg.len = count;
	   return ioctl(fd, SPI_IOC_MESSAGE(1), &msg);
   }
}
