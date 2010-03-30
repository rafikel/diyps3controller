/*
    Diy PS3 controller
    Copyright (C) 2010  Mathieu Laurendeau

    Original code from the Pan-Galactic Gargantuan Gargle Brain
    Copyright (C) 2009  Richard Burke

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
