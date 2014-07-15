//===========================================================================
// Module:  sevenseg.c
// Purpose: seven-segment display array driver
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
#include "sevenseg.h"
#include "shiftreg.h"
//-------------------[       Module Definitions        ]-------------------//
#if SEVENSEG_DIGITS < 1 || SEVENSEG_DIGITS > 8
#  error Invalid value for SEVENSEG_DIGITS
#endif
//-------------------[        Module Variables         ]-------------------//
static volatile UI8  g_nSROffset = 0;                    // shift register array offset
static volatile UI8  g_MuxPins[3] = { PIN_INVALID, };    // multiplexer selection pin array
static volatile UI8  g_nMuxPins = 0;                     // number of multiplexer pins
static volatile BOOL g_fReverse = FALSE;                 // reverse digit order?
static volatile UI8  g_Digits[SEVENSEG_DIGITS] = { 0, }; // segment value buffer
// digit-segment bit mappings
static const UI8 g_DigitMap[] =
{ 
   0b00111111, // 0
   0b00000110, // 1
   0b01011011, // 2
   0b01001111, // 3
   0b01100110, // 4
   0b01101101, // 5
   0b01111101, // 6
   0b00000111, // 7
   0b01111111, // 8
   0b01101111, // 9
};
//-------------------[        Module Prototypes        ]-------------------//
//-------------------[         Implementation          ]-------------------//
//-----------< FUNCTION: SetDigit >------------------------------------------
// Purpose:    sets the segment bitmap for a display position
// Parameters: nDigit  - the display position
//             nBitmap - the segment bitmap
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetDigit (UI8 nDigit, UI8 nBitmap)
{
   if (g_fReverse)
      g_Digits[SEVENSEG_DIGITS - nDigit - 1] = nBitmap;
   else
      g_Digits[nDigit] = nBitmap;
   if (SEVENSEG_DIGITS == 1)
      SevenSegStrobe();
}
//-----------< FUNCTION: SetDigitsR >----------------------------------------
// Purpose:    sets the segment bitmap for all positions, in reverse order
// Parameters: pDigits - the digit bitmap to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetDigitsR (const UI8* pDigits)
{
   for (UI8 i = 0; i < SEVENSEG_DIGITS; i++)
      SetDigit(i, pDigits[SEVENSEG_DIGITS - i - 1]);
}
//-----------< FUNCTION: SetError >------------------------------------------
// Purpose:    sets the display to an error value (all middle bar)
// Parameters: pDigits - the digit bitmap to assign
// Returns:    none
//---------------------------------------------------------------------------
static VOID SetError ()
{
   for (UI8 i = 0; i < SEVENSEG_DIGITS; i++)
      SetDigit(i, BitMask(SEVENSEG_MIDDLE));
}
//-----------< FUNCTION: SevenSegInit >--------------------------------------
// Purpose:    initializes the seven-segment array module
// Parameters: pConfig - module config
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegInit (PSEVENSEG_CONFIG pConfig)
{
   g_nSROffset = pConfig->nSROffset;
   for (UI8 i = 0, d = SEVENSEG_DIGITS; d > 1; i++, d /= 2)
   {
      g_MuxPins[i] = pConfig->pnMuxPins[i];
      PinSetOutput(g_MuxPins[i]);
      g_nMuxPins++;
   }
   g_fReverse = pConfig->fReverse;
}
//-----------< FUNCTION: SevenSegStrobe >------------------------------------
// Purpose:    strobes the display, swapping in the next segment bitmap
//             into the display shift register
//             this function is called externally, usually on a timer,
//             in order to make the multiplex the digits across the display
//             without requiring dedicated circuitry for each digit
// Parameters: none
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegStrobe ()
{
   static volatile UI8 g_nDigit = 0;
   // shift in the current digit
   ShiftRegWrite8(g_nSROffset, g_Digits[g_nDigit]);
   // select the digit on the multiplexer to ground its cathode
   for (UI8 i = 0; i < g_nMuxPins; i++)
      PinSet(g_MuxPins[i], BitTest(g_nDigit, i));
   // advance to the next digit
   g_nDigit = (g_nDigit + 1) % SEVENSEG_DIGITS;
}
//-----------< FUNCTION: SevenSegSetSegment >--------------------------------
// Purpose:    turns on/off a single segment in the display
// Parameters: nDigit   - the display digit to assign
//             nSegment - the segment of the digit to turn on/off
//             fValue   - true to turn on, false to turn off
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSetSegment (UI8 nDigit, UI8 nSegment, BOOL fValue)
{
   SetDigit(nDigit, BitSet(g_Digits[nDigit], nSegment, fValue));
}
//-----------< FUNCTION: SevenSegClearDigit >--------------------------------
// Purpose:    turns off a digit in the display
// Parameters: nDigit - the display digit to clear
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegClearDigit (UI8 nDigit)
{
   SetDigit(nDigit, 0);
}
//-----------< FUNCTION: SevenSegSetDigit >----------------------------------
// Purpose:    sets a digit to a numeric value
// Parameters: nDigit - the display digit to assign
//             nValue - the decimal value to set
//             fPoint - true to turn on the decimal point for the digit
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSetDigit (UI8 nDigit, UI8 nValue, BOOL fPoint)
{
   SetDigit(
      nDigit, 
      g_DigitMap[nValue] | (fPoint ? BitMask(SEVENSEG_POINT) : 0)
   );
}
//-----------< FUNCTION: SevenSegLShiftDigit >-------------------------------
// Purpose:    shifts the digits in the display forward and
//             sets the first digit in the display
// Parameters: nValue - the number to assign to first digit
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegLShiftDigit (UI8 nValue)
{
   for (UI8 i = 1; i < SEVENSEG_DIGITS; i++)
   {
      UI8 t = g_Digits[i];
      g_Digits[i] = g_Digits[i - 1];
      g_Digits[i - 1] = t;
   }
   g_Digits[0] = g_DigitMap[nValue];
}
//-----------< FUNCTION: SevenSegRShiftDigit >-------------------------------
// Purpose:    shifts the digits in the display backward and
//             sets the last digit in the display
// Parameters: nValue - the number to assign to last digit
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegRShiftDigit (UI8 nValue)
{
   for (UI8 i = 0; i < SEVENSEG_DIGITS - 1; i++)
   {
      UI8 t = g_Digits[i];
      g_Digits[i] = g_Digits[i + 1];
      g_Digits[i + 1] = t;
   }
   g_Digits[SEVENSEG_DIGITS - 1] = g_DigitMap[nValue];
}
//-----------< FUNCTION: SevenSegSet8 >--------------------------------------
// Purpose:    sets the entire display to an 8-bit decimal number
// Parameters: nValue - the number to assign to the display
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSet8 (I8 nValue)
{
   UI8 Digits[SEVENSEG_DIGITS] = { 0, };
   UI8 i = 0;
   UI8 n = nValue >= 0 ? nValue : -nValue;
   // shift the value digits into the digit buffer
   do
   {
      Digits[i++] = g_DigitMap[n % 10];
      n /= 10;
   } while (i < SEVENSEG_DIGITS && n != 0);
   // fail on overflow
   if (n != 0)
      i++;
   else if (nValue < 0)
   {
      // fail on negative overflow (including sign)
      // otherwise, include the minus sign
      if (i < SEVENSEG_DIGITS)
         Digits[i++] = BitMask(SEVENSEG_MIDDLE);
      else
         i++;
   }
   // output the result
   if (i <= SEVENSEG_DIGITS)
      SetDigitsR(Digits);
   else
      SetError();
}
//-----------< FUNCTION: SevenSegSet16 >-------------------------------------
// Purpose:    sets the entire display to a 16-bit decimal number
// Parameters: nValue - the number to assign to the display
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSet16 (I16 nValue)
{
   UI8  Digits[SEVENSEG_DIGITS] = { 0, };
   UI8  i = 0;
   UI16 n = nValue >= 0 ? nValue : -nValue;
   // shift the value digits into the digit buffer
   do
   {
      Digits[i++] = g_DigitMap[n % 10];
      n /= 10;
   } while (i < SEVENSEG_DIGITS && n != 0);
   // fail on overflow
   if (n != 0)
      i++;
   else if (nValue < 0)
   {
      // fail on negative overflow (including sign)
      // otherwise, include the minus sign
      if (i < SEVENSEG_DIGITS)
         Digits[i++] = BitMask(SEVENSEG_MIDDLE);
      else
         i++;
   }
   // output the result
   if (i <= SEVENSEG_DIGITS)
      SetDigitsR(Digits);
   else
      SetError();
}
//-----------< FUNCTION: SevenSegSet32 >-------------------------------------
// Purpose:    sets the entire display to a 32-bit decimal number
// Parameters: nValue - the number to assign to the display
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSet32 (I32 nValue)
{
   UI8  Digits[SEVENSEG_DIGITS] = { 0, };
   UI8  i = 0;
   UI32 n = nValue >= 0 ? nValue : -nValue;
   // shift the value digits into the digit buffer
   do
   {
      Digits[i++] = g_DigitMap[n % 10];
      n /= 10;
   } while (i < SEVENSEG_DIGITS && n != 0);
   // fail on overflow
   if (n != 0)
      i++;
   else if (nValue < 0)
   {
      // fail on negative overflow (including sign)
      // otherwise, include the minus sign
      if (i < SEVENSEG_DIGITS)
         Digits[i++] = BitMask(SEVENSEG_MIDDLE);
      else
         i++;
   }
   // output the result
   if (i <= SEVENSEG_DIGITS)
      SetDigitsR(Digits);
   else
      SetError();
}
//-----------< FUNCTION: SevenSegSetFP >-------------------------------------
// Purpose:    sets the entire display to a floating-point decimal number
// Parameters: nValue      - the number to assign to the display
//             nFractional - the number of fractional digits to display
// Returns:    none
//---------------------------------------------------------------------------
VOID SevenSegSetFP (F32 nValue, UI8 nFractional)
{
   UI8 Digits[SEVENSEG_DIGITS] = { 0, };
   UI8 i = 0;
   UI8 f = nFractional;
   // check for fractional digits overflow
   if (f >= SEVENSEG_DIGITS)
      i = SEVENSEG_DIGITS + 1;
   else
   {
      F32 n = nValue >= 0 ? nValue : -nValue;
      // scale the value to place fractional digits to
      // the left of the decimal point
      for (UI8 j = 0; j < nFractional; j++)
         n *= 10;
      // shift the value digits into the digit buffer
      do
      {
         UI8 p = (f == i) ? BitMask(SEVENSEG_POINT) : 0;
         Digits[i++] = g_DigitMap[(UI8)n % 10] | p;
         n /= 10;
      } while (i < SEVENSEG_DIGITS && (i <= f || (UI8)n != 0));
      // blank on overflow
      if ((UI8)n != 0)
         i++;
      else if (nValue < 0)
      {
         // blank on negative overflow (including sign)
         // otherwise, output the minus sign
         if (i < SEVENSEG_DIGITS)
            Digits[i++] = BitMask(SEVENSEG_MIDDLE);
         else
            i++;
      }
   }
   // output the result
   if (i <= SEVENSEG_DIGITS)
      SetDigitsR(Digits);
   else
      SetError();
}
