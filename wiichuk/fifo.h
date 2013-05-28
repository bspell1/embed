//===========================================================================
// Module:  fifo.h
// Purpose: FIFO queue framework
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
#ifndef __FIFO_H
#define __FIFO_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
#ifndef __AVRDEFS_H
#include "avrdefs.h"
#endif
//-------------------[      Project Include Files      ]-------------------//
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// FIFO STRUCTURES
//===========================================================================
// FIFO data structure
typedef struct tagFifo
{
   UI8   cbBuffer;
   UI8   cbOffset;
   UI8   cbLength;
   BYTE  pbBuffer[1];
} FIFO;
typedef volatile FIFO* PFIFO;
// FIFO declaration
#define DECLARE_FIFO(name, size)                                           \
   static volatile union                                                   \
   {                                                                       \
      FIFO Fifo;                                                           \
      BYTE pbBuffer[sizeof(FIFO) + size - 1];                              \
   } __FifoBuffer_##name =                                                 \
      { { .cbBuffer = size, .cbOffset = 0, .cbLength = 0 } };              \
   static PFIFO name = &__FifoBuffer_##name.Fifo
//===========================================================================
// FIFO API
//===========================================================================
inline UI8 FifoSize (PFIFO pFifo)
   { return pFifo-> cbBuffer; }
inline UI8 FifoCount (PFIFO pFifo)
   { return pFifo->cbLength; }
inline BOOL FifoIsEmpty (PFIFO pFifo)
   { return pFifo->cbLength == 0; }
inline BOOL FifoIsFull (PFIFO pFifo)
   { return pFifo->cbLength == pFifo->cbBuffer; }
inline BYTE FifoRead (PFIFO pFifo)
   {
      BYTE bData = 0;
      if (pFifo->cbLength > 0)
      {
         bData = pFifo->pbBuffer[pFifo->cbOffset];
         pFifo->cbOffset = (pFifo->cbOffset + 1) % pFifo->cbBuffer;
         pFifo->cbLength--;
      }
      return bData;
   }
inline UI8 FifoReadBlock (PFIFO pFifo, PVOID pvData, UI8 cbData)
   {
      UI8 cb = 0;
      while (cb < cbData && !FifoIsEmpty(pFifo))
         ((PBYTE)pvData)[cb++] = FifoRead(pFifo);
      return cb;
   }
inline VOID FifoWrite (PFIFO pFifo, BYTE bData)
   {
      UI8 cbOffset = (pFifo->cbOffset + pFifo->cbLength) % pFifo->cbBuffer;
      if (pFifo->cbLength < pFifo->cbBuffer)
         pFifo->cbLength++;
      else
         pFifo->cbOffset = (pFifo->cbOffset + 1) % pFifo->cbBuffer;
      pFifo->pbBuffer[cbOffset] = bData;
   }
inline VOID FifoWriteBlock (PFIFO pFifo, PCVOID pvData, UI8 cbData)
   {
      for (UI8 i = 0; i < cbData; i++)
         FifoWrite(pFifo, ((PBYTE)pvData)[i]);
   }
inline VOID FifoFlush (PFIFO pFifo)
   { pFifo->cbLength = 0; }
#endif // __FIFO_H
