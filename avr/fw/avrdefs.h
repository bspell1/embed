//===========================================================================
// Module:  avrdefs.h
// Purpose: AVR microprocessor definitions
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
#ifndef __AVRDEFS_H
#define __AVRDEFS_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <util/atomic.h>
//-------------------[      Project Include Files      ]-------------------//
//-------------------[       Module Definitions        ]-------------------//
//===========================================================================
// INTEGER TYPES
//===========================================================================
typedef int8_t                I8;
typedef uint8_t               UI8;
typedef int16_t               I16;
typedef uint16_t              UI16;
typedef int32_t               I32;
typedef uint32_t              UI32;
#define I8_MIN                INT8_MIN
#define I8_MAX                INT8_MAX
#define UI8_MIN               0
#define UI8_MAX               UINT8_MAX
#define I16_MIN               INT16_MIN
#define I16_MAX               INT16_MAX
#define UI16_MIN              0
#define UI16_MAX              UINT16_MAX
#define I32_MIN               INT32_MIN
#define I32_MAX               INT32_MAX
#define UI32_MIN              0
#define UI32_MAX              UINT32_MAX
//===========================================================================
// FLOATING POINT TYPES
//===========================================================================
typedef float                 F32;
#define F32_MIN               FLT_MIN
#define F32_MAX               FLT_MAX
#define F32_EPSILON           FLT_EPSILON
//===========================================================================
// BOOLEAN TYPES
//===========================================================================
typedef uint8_t               BOOL;
typedef BOOL                  BIT;
#define FALSE                 ((BOOL)0)
#define TRUE                  ((BOOL)1)
#define BIT_LO                ((BIT)0)
#define BIT_HI                ((BIT)1)
#define BitMask(b)            (1 << (b))
#define BitUnmask(b)          (~BitMask(b))
#define BitTest(v, b)         (((v) & BitMask(b)) ? BIT_HI : BIT_LO)
#define BitSetHi(v, b)        ((v) | BitMask(b))
#define BitSetLo(v, b)        ((v) & BitUnmask(b))
#define BitSet(v, b, t)       ((t) ? BitSetHi(v, b) : BitSetLo(v, b))
//===========================================================================
// BUFFER TYPES
//===========================================================================
typedef void                  VOID;
typedef VOID*                 PVOID;
typedef const VOID*           PCVOID;
typedef uint8_t               BYTE;
typedef BYTE*                 PBYTE;
typedef const BYTE*           PCBYTE;
typedef size_t                BSIZE;
#define BYTE_MIN              UINT8_MIN
#define BYTE_MAX              UINT8_MAX
#define BSIZE_MIN             0
#define BSIZE_MAX             SIZE_MAX
//===========================================================================
// STRING TYPES
//===========================================================================
typedef char                  CHAR;
typedef CHAR*                 PSTR;
typedef const CHAR*           PCSTR;
// string defines
#ifndef STR
#  define STR(sz)             sz
#endif
//===========================================================================
// MIN/MAX DEFINITIONS
//===========================================================================
#ifndef Min
#  define Min(x, y)           (((x) < (y)) ? (x) : (y))
#endif
#ifndef Max
#  define Max(x, y)           (((x) > (y)) ? (x) : (y))
#endif
#ifndef Clamp
#  define Clamp(x, min, max)  (Min(Max((x), (min)), (max)))
#endif
#ifndef Map
#  define Map(x, imin, imax, omin, omax)                                   \
      (((x) - (imin)) * ((omax) - (omin)) / ((imax) - (imin)) + (omin))
#endif
#ifndef MapClamp
#  define MapClamp(x, imin, imax, omin, omax)                              \
      Map(Clamp(x, imin, imax), imin, imax, omin, omax)
#endif
//===========================================================================
// MEMORY OPERATIONS
//===========================================================================
#ifndef memzero
#  define memzero(pv, cb)     memset((pv), 0, (cb))
#endif
//===========================================================================
// COMPILER OPTIONS
//===========================================================================
#ifndef IgnoreParam
#  define IgnoreParam(p)      (void)(p)
#endif
//===========================================================================
// AVR PINS
//===========================================================================
// pin direction, input, output register vectors
static __attribute__((unused)) volatile uint8_t* __ppbAvrDR[] = { &DDRB,  &DDRC,  &DDRD  };
static __attribute__((unused)) volatile uint8_t* __ppbAvrDI[] = { &PINB,  &PINC,  &PIND  };
static __attribute__((unused)) volatile uint8_t* __ppbAvrDO[] = { &PORTB, &PORTC, &PORTD };
// AVR pin definitions
#define PIN_INVALID           0xFF
#define PIN_B0                0
#define PIN_B1                1
#define PIN_B2                2
#define PIN_B3                3
#define PIN_B4                4
#define PIN_B5                5
#define PIN_B6                6
#define PIN_B7                7
#define PIN_C0                8
#define PIN_C1                9
#define PIN_C2                10
#define PIN_C3                11
#define PIN_C4                12
#define PIN_C5                13
#define PIN_C6                14
#define PIN_D0                16
#define PIN_D1                17
#define PIN_D2                18
#define PIN_D3                19
#define PIN_D4                20
#define PIN_D5                21
#define PIN_D6                22
#define PIN_D7                23
// AVR 328P pin aliases
#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
#  define PIN_RESET           PIN_C6
#  define PIN_T0              PIN_D4
#  define PIN_T1              PIN_D5
#  define PIN_TOSC1           PIN_B6
#  define PIN_TOSC2           PIN_B7
#  define PIN_XTAL1           PIN_B6
#  define PIN_XTAL2           PIN_B7
#  define PIN_ICP1            PIN_B0
#  define PIN_CLKO            PIN_B0
#  define PIN_OC0A            PIN_D6
#  define PIN_OC0B            PIN_D5
#  define PIN_OC1A            PIN_B1
#  define PIN_OC1B            PIN_B2
#  define PIN_OC2A            PIN_B3
#  define PIN_OC2B            PIN_D3
#  define PIN_INT0            PIN_D2
#  define PIN_INT1            PIN_D3
#  define PIN_PCINT0          PIN_B0
#  define PIN_PCINT1          PIN_B1
#  define PIN_PCINT2          PIN_B2
#  define PIN_PCINT3          PIN_B3
#  define PIN_PCINT4          PIN_B4
#  define PIN_PCINT5          PIN_B5
#  define PIN_PCINT6          PIN_B6
#  define PIN_PCINT7          PIN_B7
#  define PIN_PCINT8          PIN_C0
#  define PIN_PCINT9          PIN_C1
#  define PIN_PCINT10         PIN_C2
#  define PIN_PCINT11         PIN_C3
#  define PIN_PCINT12         PIN_C4
#  define PIN_PCINT13         PIN_C5
#  define PIN_PCINT14         PIN_C6
#  define PIN_PCINT16         PIN_D0
#  define PIN_PCINT17         PIN_D1
#  define PIN_PCINT18         PIN_D2
#  define PIN_PCINT19         PIN_D3
#  define PIN_PCINT20         PIN_D4
#  define PIN_PCINT21         PIN_D5
#  define PIN_PCINT22         PIN_D6
#  define PIN_PCINT23         PIN_D7
#  define PIN_ADC0            PIN_C0
#  define PIN_ADC1            PIN_C1
#  define PIN_ADC2            PIN_C2
#  define PIN_ADC3            PIN_C3
#  define PIN_ADC4            PIN_C4
#  define PIN_ADC5            PIN_C5
#  define PIN_AIN0            PIN_D6
#  define PIN_AIN1            PIN_D7
#  define PIN_XCK             PIN_D4
#  define PIN_TXD             PIN_D1
#  define PIN_RXD             PIN_D0
#  define PIN_SCK             PIN_B5
#  define PIN_MOSI            PIN_B3
#  define PIN_MISO            PIN_B4
#  define PIN_SS              PIN_B2
#  define PIN_SCL             PIN_C5
#  define PIN_SDA             PIN_C4
#endif // defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328__)
// arduino pins
#define PIN_ARDUINO_LED       PIN_B5
#define PIN_ARDUINO_D0        PIN_D0
#define PIN_ARDUINO_D1        PIN_D1
#define PIN_ARDUINO_D2        PIN_D2
#define PIN_ARDUINO_D3        PIN_D3
#define PIN_ARDUINO_D4        PIN_D4
#define PIN_ARDUINO_D5        PIN_D5
#define PIN_ARDUINO_D6        PIN_D6
#define PIN_ARDUINO_D7        PIN_D7
#define PIN_ARDUINO_D8        PIN_B0
#define PIN_ARDUINO_D9        PIN_B1
#define PIN_ARDUINO_D10       PIN_B2
#define PIN_ARDUINO_D11       PIN_B3
#define PIN_ARDUINO_D12       PIN_B4
#define PIN_ARDUINO_D13       PIN_B5
#define PIN_ARDUINO_A0        PIN_C0
#define PIN_ARDUINO_A1        PIN_C1
#define PIN_ARDUINO_A2        PIN_C2
#define PIN_ARDUINO_A3        PIN_C3
#define PIN_ARDUINO_A4        PIN_C4
#define PIN_ARDUINO_A5        PIN_C5
// register manipulation
#define RegGet(r, b)          BitTest(r, b)
#define RegSetLo(r, b)        (r) &= BitUnmask(b)
#define RegSetHi(r, b)        (r) |= BitMask(b)
#define RegSet(r, b, v)       do { if ((v)) RegSetHi(r, b); else RegSetLo(r, b); } while (0)
#define RegToggle(r, b)       (r) ^= BitMask(b)
// pin configuration
#define PIN_MODE_INPUT        0
#define PIN_MODE_OUTPUT       1
// pin register access
#define __PIN_DREG(p)         (*__ppbAvrDR[(p) / 8])
#define __PIN_IREG(p)         (*__ppbAvrDI[(p) / 8])
#define __PIN_OREG(p)         (*__ppbAvrDO[(p) / 8])
#define __PIN_BIT(p)          ((p) % 8)
#define PinSetMode(p, m)      RegSet(__PIN_DREG(p), __PIN_BIT(p), m)
#define PinSetInput(p)        RegSet(__PIN_DREG(p), __PIN_BIT(p), PIN_MODE_INPUT)
#define PinSetOutput(p)       RegSet(__PIN_DREG(p), __PIN_BIT(p), PIN_MODE_OUTPUT)
#define PinSetPullUp(p)       PinSetInput(p); PinSetHi(p)
#define PinSetTriState(p)     PinSetInput(p); PinSetLo(p)
#define PinRead(p)            RegGet(__PIN_IREG(p), __PIN_BIT(p))
#define PinGet(p)             RegGet(__PIN_OREG(p), __PIN_BIT(p))
#define PinSetLo(p)           RegSetLo(__PIN_OREG(p), __PIN_BIT(p))
#define PinSetHi(p)           RegSetHi(__PIN_OREG(p), __PIN_BIT(p))
#define PinSet(p, v)          RegSet(__PIN_OREG(p), __PIN_BIT(p), v)
#define PinToggle(p)          RegToggle(__PIN_OREG(p), __PIN_BIT(p))
#define PinPulseHiLo(p)       PinSetHi(p); PinSetLo(p)
#define PinPulseLoHi(p)       PinSetLo(p); PinSetHi(p)
#define PinPulse(p)           PinPulseHiLo(p)
//===========================================================================
// AVR CLOCKS
//===========================================================================
#define AvrClkCtcTop(ps, freq)    (F_CPU / (ps) / (freq) - 1)
#define AvrClk0Scale(x) (                                                  \
   (x == 1) ?    (BitMask(CS00)) :                                         \
   (x == 8) ?    (BitMask(CS01)) :                                         \
   (x == 64) ?   (BitMask(CS01) | BitMask(CS00)) :                         \
   (x == 256) ?  (BitMask(CS02)) :                                         \
   (x == 1024) ? (BitMask(CS02) | BitMask(CS00)) : 0)
#define AvrClk1Scale(x) (                                                  \
   (x == 1) ?    (BitMask(CS10)) :                                         \
   (x == 8) ?    (BitMask(CS11)) :                                         \
   (x == 64) ?   (BitMask(CS11) | BitMask(CS10)) :                         \
   (x == 256) ?  (BitMask(CS12)) :                                         \
   (x == 1024) ? (BitMask(CS12) | BitMask(CS10)) : 0)
#define AvrClk2Scale(x) (                                                  \
   (x == 1) ?    (BitMask(CS20)) :                                         \
   (x == 8) ?    (BitMask(CS21)) :                                         \
   (x == 32) ?   (BitMask(CS21) | BitMask(CS20)) :                         \
   (x == 64) ?   (BitMask(CS22)) :                                         \
   (x == 128) ?  (BitMask(CS22) | BitMask(CS20)) :                         \
   (x == 256) ?  (BitMask(CS22) | BitMask(CS21)) :                         \
   (x == 1024) ? (BitMask(CS22) | BitMask(CS21) | BitMask(CS20)) : 0)
//===========================================================================
// AVR OPERATIONS
//===========================================================================
inline VOID AvrAbort ()
   { wdt_enable(WDTO_1S); for ( ; ; ) { } }
#endif // __AVRDEFS_H
