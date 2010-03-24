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
#ifndef __PS2MS_H__
#define __PS2MS_H__

/*
 ********************************************************************************
 * Resources
 ********************************************************************************
 */
#define MS_ClockInterrupt               MS_INT_VECTOR
#define MS_TimerInterrupt               TIMER1_COMPA_vect
#define ms_clock_enable_interrupt()     ( sbi( EIMSK, MS_INT ) )
#define ms_clock_disable_interrupt()    ( cbi( EIMSK, MS_INT ) )
#define ms_clock_clear_interrupt()      ( sbi( EIFR, INTF6 ) )
#define ms_clock_rising_edge()          ( sbi( EICRB, ISC61 ) | sbi( EICRB,ISC60 ) )
#define ms_clock_falling_edge()         ( sbi( EICRB, ISC61 ) )
#define ms_timer_enable_interrupt()     ( sbi( TIMSK1, OCIE1A ) )
#define ms_timer_disable_interrupt()    ( cbi( TIMSK1, OCIE1A ) )

#define MS_INT                          INT6
#define MS_INT_VECTOR                   INT6_vect

#define MS_DATA_PORT                    PORTE
#define MS_DATA_DDR                     DDRE
#define MS_DATA_PIN                     PINE
#define MS_DATA_BIT                     PE0

#define MS_CLOCK_PORT                   PORTE
#define MS_CLOCK_DDR                    DDRE
#define MS_CLOCK_PIN                    PINE
#define MS_CLOCK_BIT                    PE6

/*
 ********************************************************************************
 * Data Structures
 ********************************************************************************
 */

// Vector translation. Uses whole numbers to represent a fraction with offset.
typedef struct _scale_t
{
    uint8_t offset;
    uint16_t numerator;
    uint16_t denominator;
} scale_t;

// Vector translation applied between a lower and upper value.
typedef struct _range_t
{
    int8_t lower;
    int8_t upper;
    scale_t scale;
} range_t;

// Protocol unit assembled from the stream of bytes sent by the PS2 driver.
#define MOUSE_VECTOR_SIZE               4

typedef struct _packet_t
{
    uint8_t size;
    union
    {
        struct
        {
            uint8_t bytes[MOUSE_VECTOR_SIZE];
        };
        struct
        {
            union
            {
                uint8_t header;
                struct
                {
                    uint8_t left: 1;
                    uint8_t right: 1;
                    uint8_t middle: 1;
                    uint8_t one: 1;
                    uint8_t xsign: 1;
                    uint8_t ysign: 1;
                    uint8_t xoverflow: 1;
                    uint8_t yoverflow: 1;
                };
            };
            uint8_t x;
            uint8_t y;
            uint8_t z;
        };
    };
} packet_t;

// Vector magnitude.
typedef struct _vector_t
{
    struct
    {
        uint8_t sign : 1;
        uint8_t overflow : 1;
        uint8_t label : 6;
    };
    uint8_t data;
} vector_t;

#define MOVING_AVERAGE_BITS         2
#define MOVING_AVERAGE_SIZE         ( 1 << MOVING_AVERAGE_BITS )
#define MOVING_AVERAGE_MASK         ( MOVING_AVERAGE_SIZE - 1 )

typedef struct _average_t
{
    uint8_t index;
    uint8_t data[MOVING_AVERAGE_SIZE];
} average_t;

typedef struct _trigger_t
{
    volatile uint8_t timeout;
    volatile uint8_t timer;
    volatile uint8_t action;
} trigger_t;

#define MS_BUFSIZE                  2

typedef struct _driver_t
{
    volatile uint8_t bit_n;
    volatile uint8_t buffer;
    volatile uint8_t error;
    uint8_t data[MS_BUFSIZE];
    ringbuff_t ringbuffer;
} driver_t;

typedef struct _config_t
{
    uint8_t frequency;
    uint8_t resolution: 2;
    uint8_t scale: 2;
} config_t;

typedef struct _filter
{
    uint8_t state;
    uint8_t retry;
    uint8_t size;
    uint8_t *table;
} filter_t;

typedef struct _dispatch
{
    struct
    {
        uint8_t left: 1;
        uint8_t right: 1;
        uint8_t middle: 1;
        uint8_t up: 1;
        uint8_t down: 1;
    };
    uint8_t hysteresis;
    average_t averagex;
    average_t averagey;
} dispatch_t;

/*
 ****************************************************************************************************
 * Mouse Events
 *
 *   ,----------------------------------------------------  Device : Mouse
 *   |             ,-------------------------------------- Trigger : MouseTrigger_t
 *   v             v             v------------------------   Value : MouseValue_t
 *  ------------- ------------- -------------------------
 * | D3 D2 D1 D0 | T3 T2 T1 T0 | V7 V6 V5 V4 V3 V2 V1 V0 |
 *  ------------- ------------- -------------------------
 * |            Key            |         Value           |
 *  --------------------------- -------------------------
 *
 ****************************************************************************************************
 */
typedef enum { MKeyPress, MKeyRelease, MDeltaX, MDeltaY } MouseTrigger_t;
typedef enum { MVector, MLeft, MRight, MMiddle, MWheelDown, MWheelUp } MouseValue_t;

typedef enum { Count1, Count2, Count4, Count8 } Resolution_t;
typedef enum { Rate10 = 10, Rate20 = 20, Rate40 = 40, Rate60 = 60, Rate80 = 80, Rate100 = 100, Rate200 = 200 } Frequency_t;
typedef enum { Unity = 1, Double = 2 } Scaling_t;

// Mouse Events
#define MS_ButtonPress(v)       EVA_NewEvent( EVA_NewEventKey( Mouse, MKeyPress ), ( v ) )
#define MS_ButtonRelease(v)     EVA_NewEvent( EVA_NewEventKey( Mouse, MKeyRelease ), ( v ) )
#define MS_DeltaX(v)            EVA_NewEvent( EVA_NewEventKey( Mouse, MDeltaX ), ( v ) )
#define MS_DeltaY(v)            EVA_NewEvent( EVA_NewEventKey( Mouse, MDeltaY ), ( v ) )
#define MS_FindByKey(v)         ( ( EVA_EventDevice( v ) == Mouse )? ( ( EVA_EventTrigger( v ) == MDeltaX ) || ( EVA_EventTrigger( v ) == MDeltaY ) ) : 0 )

/*
 ********************************************************************************
 * Public function declarations
 ********************************************************************************
 */
extern void MS_Init( void );
extern void MS_Action( uint16_t action );
extern void MS_MouseReset( void );
extern void MS_SetResolution( uint8_t resolution );
extern void MS_SetFrequency( uint8_t frequency );
extern void MS_SetScaling( uint8_t scale );
extern void MS_EventTask( void );

#endif
