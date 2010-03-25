/*
 ********************************************************************************
 * Copyright (c) 2009 Richard Burke
 * Copyright (c) 2010 Mathieu Laurendeau
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * * Neither the name of the copyright holders nor the names of
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * $Rev: 223 $
 * $Author: rburke $
 * $LastChangedDate: 2009-11-27 01:31:22 -0500 (Fri, 27 Nov 2009) $
 ********************************************************************************
 */
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/*
 ********************************************************************************
 * Resources
 ********************************************************************************
 */
#define KB_ClockInterrupt               KB_INT_VECTOR

#define kb_clock_enable_interrupt()     ( sbi( EIMSK, KB_INT ) )
#define kb_clock_disable_interrupt()    ( cbi( EIMSK, KB_INT ) )
#define kb_clock_clear_interrupt()      ( sbi( EIFR, INTF7 ) )
#define kb_clock_rising_edge()          ( sbi( EICRB, ISC71 ) | sbi( EICRB,ISC70 ) )
#define kb_clock_falling_edge()         ( sbi( EICRB, ISC71 ) )

#define KB_INT                          INT7
#define KB_INT_VECTOR                   INT7_vect

#define KB_DATA_PORT                    PORTE
#define KB_DATA_DDR                     DDRE
#define KB_DATA_PIN                     PINE
#define KB_DATA_BIT                     PE1

#define KB_CLOCK_PORT                   PORTE
#define KB_CLOCK_DDR                    DDRE
#define KB_CLOCK_PIN                    PINE
#define KB_CLOCK_BIT                    PE7

#define KB_BUFSIZE                      8

/*
 ****************************************************************************************************
 * Keyboard Events
 *
 *   ,----------------------------------------------------  Device : Keyboard
 *   |             ,-------------------------------------- Trigger : KeyTrigger_t -> Break ( F0 ), Extended ( E0 )
 *   v             v             v------------------------   Value : Keys_t
 *  ------------- ------------- -------------------------
 * | D3 D2 D1 D0 | T3 T2 E0 F0 | V7 V6 V5 V4 V3 V2 V1 V0 |
 *  ------------- ------------- -------------------------
 * |            Key            |         Value           |
 *  --------------------------- -------------------------
 *
 ****************************************************************************************************
 */
typedef enum { KeyPress, KeyRelease, EKeyPress, EKeyRelease } KeyTrigger_t;
typedef enum
{
    AKey = 0x1C, BKey = 0x32, CKey = 0x21, DKey = 0x23, EKey = 0x24, FKey = 0x2B, GKey = 0x34, HKey = 0x33, IKey = 0x43, JKey = 0x3B,
    KKey = 0x42, LKey = 0x4B, MKey = 0x3A, NKey = 0x31, OKey = 0x44, PKey = 0x4D, QKey = 0x15, RKey = 0x2D, SKey = 0x1B, TKey = 0x2C,
    UKey = 0x3C, VKey = 0x2A, WKey = 0x1D, XKey = 0x22, YKey = 0x35, ZKey = 0x1A, D0Key = 0x45, D1Key = 0x16, D2Key = 0x1E, D3Key = 0x26,
    D4Key = 0x25, D5Key = 0x2E, D6Key = 0x36, D7Key = 0x3D, D8Key = 0x3E, D9Key = 0x46, TildeKey = 0x0E, MinusKey = 0x4E,
    PlusKey = 0x55, PipeKey = 0x5D, BackKey = 0x66, SpaceKey = 0x29, TabKey = 0x0D, CapsLockKey = 0x58, LeftShiftKey = 0x12,
    LeftControlKey = 0x14, LeftWindowsKey = 0x1F, LeftAltKey = 0x11, RightShiftKey = 0x59, RightControlKey = 0x14,
    RightWindowsKey = 0x1F, RightAltKey = 0x11, AppsKey = 0x2F, EnterKey = 0x5A, EscapeKey = 0x76, F1Key = 0x05, F2Key = 0x06,
    F3Key = 0x04, F4Key = 0x0C, F5Key = 0x03, F6Key = 0x0B, F7Key = 0x83, F8Key = 0x0A, F9Key = 0x01, F10Key = 0x09, F11Key = 0x78,
    F12Key = 0x07, ScrollLockKey = 0x7E, OpenBracketKey = 0x54, InsertKey = 0x70, HomeKey = 0x6C, PageUpKey = 0x7D,
    DeleteKey = 0x71, EndKey = 0x69, PageDownKey = 0x7A, UpKey = 0x75, LeftKey = 0x6B, DownKey = 0x72, RightKey = 0x74,
    NumLockKey = 0x77, DivideKey = 0x4A, MultiplyKey = 0x7C, SubtractKey = 0x7B, AddKey = 0x79, DecimalKey = 0x71,
    NumPad0Key = 0x70, NumPad1Key = 0x69, NumPad2Key = 0x72, NumPad3Key = 0x7A, NumPad4Key = 0x6B, NumPad5Key = 0x73,
    NumPad6Key = 0x74, NumPad7Key = 0x6C, NumPad8Key = 0x75, NumPad9Key = 0x7D, CloseBracketKey = 0x5B, SemicolonKey = 0x4C,
    QuoteKey = 0x52, CommaKey = 0x41, PeriodKey = 0x49, SlashKey = 0x4A, NextTrackKey = 0x4D, PreviousTrackKey = 0x15,
    StopKey = 0x3B, PlayPauseKey = 0x34, MuteKey = 0x23, VolumeUpKey = 0x32, VolumeDownKey = 0x21, MediaSelectKey = 0x50,
    LaunchMailKey = 0x48, CalculatorKey = 0x2B, ComputerKey = 0x40, BrowserSearchKey = 0x10, BrowserHomeKey = 0x3A,
    BrowserBackKey = 0x38, BrowserForwardKey = 0x30, BrowserStopKey = 0x28, BrowserRefreshKey = 0x20, BrowserFavoritesKey = 0x18,
    PowerKey = 0x37, SleepKey = 0x3F, WakeKey = 0x5E, Track1Key = 0x44, Track2Key = 0x1E, RecordKey = 0x2E,
    ClubHPKey = 0x2C, HelpKey = 0x4B, SearchKey = 0x10, InternetKey = 0x33
} Keys_t;

// Keyboard Events
#define KB_KeyPress(v)      EVA_NewEvent( EVA_NewEventKey( Keyboard, KeyPress ), ( v ) )
#define KB_KeyRelease(v)    EVA_NewEvent( EVA_NewEventKey( Keyboard, KeyRelease ), ( v ) )
#define KB_EKeyPress(v)     EVA_NewEvent( EVA_NewEventKey( Keyboard, EKeyPress ), ( v ) )
#define KB_EKeyRelease(v)   EVA_NewEvent( EVA_NewEventKey( Keyboard, EKeyRelease ), ( v ) )
#define KB_FindByKey(e)     (0)

/*
 ********************************************************************************
 * Enums
 ********************************************************************************
 */
#define KB_F0               0
#define KB_E0               1

#define KB_LED_ERROR        10000
#define KB_LED_NORMAL       50000

/*
 ********************************************************************************
 * Public function declarations
 ********************************************************************************
 */
extern void KB_Init( void );
extern void KB_Action( uint16_t );
extern void KB_ErrorStateLed( void );
extern void KB_NormalStateLed( void );
extern void KB_EventTask( void );

#endif
