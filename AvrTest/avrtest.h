//===========================================================================
// Module:  avrtest.h
// Purpose: AVR microprocessor test laboratory
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
#ifndef __AVRTEST_H
#define __AVRTEST_H
//-------------------[       Pre Include Defines       ]-------------------//
//-------------------[      Library Include Files      ]-------------------//
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
//-------------------[      Project Include Files      ]-------------------//
//-------------------[       Module Definitions        ]-------------------//
// data direction registers
#define DDRPB0                DDRB
#define DDRPB1                DDRB
#define DDRPB2                DDRB
#define DDRPB3                DDRB
#define DDRPB4                DDRB
#define DDRPB5                DDRB
#define DDRPB6                DDRB
#define DDRPB7                DDRB
#define DDRPC0                DDRC
#define DDRPC1                DDRC
#define DDRPC2                DDRC
#define DDRPC3                DDRC
#define DDRPC4                DDRC
#define DDRPC5                DDRC
#define DDRPC6                DDRC
#define DDRPD0                DDRD
#define DDRPD1                DDRD
#define DDRPD2                DDRD
#define DDRPD3                DDRD
#define DDRPD4                DDRD
#define DDRPD5                DDRD
#define DDRPD6                DDRD
#define DDRPD7                DDRD
// port registers 
#define PORTPB0               PORTB
#define PORTPB1               PORTB
#define PORTPB2               PORTB
#define PORTPB3               PORTB
#define PORTPB4               PORTB
#define PORTPB5               PORTB
#define PORTPB6               PORTB
#define PORTPB7               PORTB
#define PORTPC0               PORTC
#define PORTPC1               PORTC
#define PORTPC2               PORTC
#define PORTPC3               PORTC
#define PORTPC4               PORTC
#define PORTPC5               PORTC
#define PORTPC6               PORTC
#define PORTPD0               PORTD
#define PORTPD1               PORTD
#define PORTPD2               PORTD
#define PORTPD3               PORTD
#define PORTPD4               PORTD
#define PORTPD5               PORTD
#define PORTPD6               PORTD
#define PORTPD7               PORTD
// pin registers  
#define PINPB0                PINB
#define PINPB1                PINB
#define PINPB2                PINB
#define PINPB3                PINB
#define PINPB4                PINB
#define PINPB5                PINB
#define PINPB6                PINB
#define PINPB7                PINB
#define PINPC0                PINC
#define PINPC1                PINC
#define PINPC2                PINC
#define PINPC3                PINC
#define PINPC4                PINC
#define PINPC5                PINC
#define PINPC6                PINC
#define PINPD0                PIND
#define PINPD1                PIND
#define PINPD2                PIND
#define PINPD3                PIND
#define PINPD4                PIND
#define PINPD5                PIND
#define PINPD6                PIND
#define PINPD7                PIND
// pin input/output modes
#define PINMODE_INPUT         0
#define PINMODE_OUTPUT        1
// pin digital values   
#define PIN_HI                1
#define PIN_LO                0
// register manipulation
#define BITMASK(b)            (1 << b)
#define REGBIT(r, b)          ((r & BITMASK(b)) ? 1 : 0)
#define REGSETBIT(r, b, v)    if ((v)) r |= BITMASK(b); else r &= ~BITMASK(b)
#define REGSETBITHI(r, b)     r |= BITMASK(b)
#define REGSETBITLO(r, b)     r &= ~BITMASK(b)
#define REGTOGGLEBIT(r, b)    r ^= BITMASK(b)
// pin input/output assignment
#define PINSETMODE(p, m)      REGSETBIT(DDR##p, p, m)
#define PINSETOUTPUT(p)       REGSETBITHI(DDR##p, p)
#define PINSETINPUT(p)        REGSETBITLO(DDR##p, p)
// pin value assignment
#define PINSET(p, v)          REGSETBIT(PORT##p, p, v)
#define PINSETHI(p)           REGSETBITHI(PORT##p, p)
#define PINSETLO(p)           REGSETBITLO(PORT##p, p)
#define PINTOGGLE(p)          REGTOGGLEBIT(PORT##p, p)
#define PINPULSEHILO(p)       REGSETBITHI(PORT##p, p); REGSETBITLO(PORT##p, p)
#define PINPULSELOHI(p)       REGSETBITLO(PORT##p, p); REGSETBITHI(PORT##p, p)
#define PINPULSE(p)           REGSETBITHI(PORT##p, p); REGSETBITLO(PORT##p, p)
#endif // __AVRTEST_H
