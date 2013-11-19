//===========================================================================
// Module:  spimast.h
// Purpose: AVR SPI master driver
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
#ifndef __SPIMAST_H
#define __SPIMAST_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
//-------------------[      Project Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// SPI CONFIGURATION
// . SPI_FREQUENCY            frequency of the SCK clock, in Hz
// . SPI_LSB                  set to TRUE to enable least-signifcant-bit first
// . SPI_CPOL                 SPI mode for clock polarity
// . SPI_CPHA                 SPI mode for clock phase
// . SPI_BUFFER_SIZE          size of the SPI send/receive buffer, in bytes
//===========================================================================
#ifndef SPI_FREQUENCY
#  define SPI_FREQUENCY       8000000
#endif
#ifndef SPI_LSB
#  define SPI_LSB             FALSE
#endif      
#ifndef SPI_CPOL     
#  define SPI_CPOL            0
#endif      
#ifndef SPI_CPHA     
#  define SPI_CPHA            0
#endif
#ifndef SPI_BUFFER_SIZE
#  define SPI_BUFFER_SIZE     16
#endif
//===========================================================================
// SPI INTERFACE
//===========================================================================
// SPI callback
typedef VOID (*SPI_CALLBACK)  ();
// SPI API
VOID     SpiInit           ();
BOOL     SpiIsBusy         ();
VOID     SpiWait           ();
VOID     SpiBeginSendRecv  (UI8          nSsPin, 
                            PCVOID       pvSend, 
                            BSIZE        cbSend,
                            BSIZE        cbRecv,
                            SPI_CALLBACK pfnCallback);
UI8      SpiEndSendRecv    (PVOID        pvRecv, 
                            UI8          cbRecv);
UI8      SpiSendRecv       (UI8          nSsPin, 
                            PCVOID       pvSend, 
                            BSIZE        cbSend,
                            PVOID        pvRecv, 
                            BSIZE        cbRecv);
// SPI one-way helpers
inline VOID SpiSend (UI8 nSsPin, PCVOID pvSend, BSIZE cbSend)
   { SpiBeginSendRecv(nSsPin, pvSend, cbSend, 0, NULL); }
inline VOID SpiBeginRecv (UI8 nSsPin, BSIZE cbRecv, SPI_CALLBACK pfnCallback)
   { SpiBeginSendRecv(nSsPin, NULL, 0, cbRecv, pfnCallback); }
inline UI8 SpiEndRecv (PVOID pvRecv, BSIZE cbRecv)
   { return SpiEndSendRecv(pvRecv, cbRecv); }
inline UI8 SpiRecv (UI8 nSsPin, PVOID pvRecv, BSIZE cbRecv)
   { return SpiSendRecv(nSsPin, NULL, 0, pvRecv, cbRecv); }
#endif // __SPIMAST_H
