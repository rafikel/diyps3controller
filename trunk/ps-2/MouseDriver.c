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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <util/parity.h>

#include "Libdefs.h"
#include "RingBuffer.h"
#include "EventAction.h"
#include "KeyboardDriver.h"
#include "MouseDriver.h"

#include "usb_joystick.h"
#include "led.h"

#include "math.h"

/*
 ********************************************************************************
 * Mouse startup sequence. On power-up the state is 02. The mouse performs
 * initialization and self-diagnostics. The mouse then sends a BAT completion code
 * of 0xAA (BAT successful). Following the BAT completion code (0xAA), the mouse
 * sends its device ID of 0x00.
 * 
 * After power-on or reset the Microsoft Intellimouse operates just like a standard
 * PS/2 mouse (ie, it uses a 3-byte movement data packet, responds to all commands
 * in the same way as a standard PS/2 mouse, and reports a device ID of 0x00.) To
 * enable the scrolling wheel, the host sends the following command sequence:
 *		1. Set sample rate 200 
 *		2. Set sample rate 100 
 *		3. Set sample rate 80
 * 
 * The host then issues the "Get device ID" (0xF2) command and waits for a response.
 * If a standard PS/2 mouse (i.e., non-Intellimouse) is attached, it will respond
 * with a device ID of 0x00. In this case, the host will recognize the fact that the
 * mouse does not have a scrolling wheel and will continue to treat it as a standard PS/2
 * mouse. However, if a Microsoft Intellimouse is attached, it will respond with an
 * ID of 0x03. This tells the host that the attached pointing device has a scrolling
 * wheel, and the host will then expect the mouse to use the 4-byte movement data packet.
 *
 * During power-on-reset state is 2 and the sequence starts with the mouse sending
 * a BAT code. For soft reset, the state is 0. The mouse sends 0xFA and then follows
 * with BAT. State 2 and state 0 are the only valid entry points into the state
 * machine.
 ********************************************************************************
 */
static uint8_t ms_state_table[] PROGMEM = {
	// RESET SEQUENCE
	// state rx'd  tx'd
	/* 00 */ 0xFA, 0x00,	// ACK
	/* 02 */ 0xAA, 0x00,	// BAT
	/* 04 */ 0x00, 0xFF,	// Mouse ID -> Send Reset
	/* 06 */ 0xFA, 0x00,	// ACK
	/* 08 */ 0xAA, 0x00,	// BAT
	/* 10 */ 0x00, 0xFF,	// Mouse ID -> Send Reset
	/* 12 */ 0xFA, 0x00,	// ACK
	/* 14 */ 0xAA, 0x00,	// BAT
	/* 16 */ 0x00, 0xFF,	// Mouse ID -> Send Reset
	/* 18 */ 0xFA, 0x00,	// ACK
	/* 20 */ 0xAA, 0x00,	// BAT
	/* 22 */ 0x00, 0xF3,	// Set Sample Rate	| Attempt to Enter Microsoft 
	/* 24 */ 0xFA, 200,		// Decimal 200		| Scrolling Mouse mode
	/* 26 */ 0xFA, 0xF3,	// Set Sample Rate	|
	/* 28 */ 0xFA, 100,		// Decimal 100		|
	/* 30 */ 0xFA, 0xF3,	// Set Sample Rate	|
	/* 32 */ 0xFA, 80,		// Decimal 80		|
	/* 34 */ 0xFA, 0xF2,	// Read Device Type |
	/* 36 */ 0xFA, 0x00,	// ACK				|
	/* 38 */ 0x03, 0xE8,	// Set Resolution | Response 03 if microsoft scrolling mouse
	/* 40 */ 0xFA, 0xD1,	// ms_resolution (Default = 8 counts / mm)
	/* 42 */ 0xFA, 0xD2,	// Set scaling ms_vector (Default 1:1)
	/* 44 */ 0xFA, 0xF3,	// Set Sample Rate
	/* 46 */ 0xFA, 0xD3,	// ms_frequency (Default = 100 Samples / Second)
	/* 48 */ 0xFA, 0xF4,	// Enable device
	/* 50 */ 0xFA, 0x00,	// ACK
};

/*
 ********************************************************************************
 * Local Variables
 *******************************************************************************
 */
static driver_t ms_driver;		// MS_ClockInterrupt variables
static trigger_t ms_trigger;	// MS_TimerInterrupt variables
static packet_t ms_packet;		// Current packet in the EventFilter
static dispatch_t ms_dispatch;	// Average wiper values and current button state
static config_t ms_config;		// Mouse configuration
static filter_t ms_filter;		// Event filter state

/*
 ********************************************************************************
 * Private Functions
 ********************************************************************************
 */
static vector_t *AbsoluteValue( vector_t *vector );
static void EventDispatcher( packet_t *packet );
static void EventFilter( uint8_t scancode );
static uint8_t GetScancode( void );
static uint8_t HasScancode( void );
static void ResetState( void );
static void SendByte( uint8_t data );
static void SetMouseDefault( void );
static void TimerInit( void );
static vector_t *VectorX( packet_t *packet, vector_t *vector );
static vector_t *VectorY( packet_t *packet, vector_t *vector );

/*
 ********************************************************************************
 * MS_Init
 ********************************************************************************
 */
void MS_Init( void )
{
	ms_driver.bit_n = 1;
	ms_driver.buffer = 0;
	ms_driver.error = 0;
	RING_Initialize( &ms_driver.ringbuffer, ms_driver.data, sizeof( ms_driver.data ) );

	ms_trigger.timeout = 15;
	ms_trigger.timer = 0;
	ms_trigger.action = 0;

	ms_packet.size = 0;

	ms_dispatch.averagex.index = 0;
	ms_dispatch.averagey.index = 0;

	ms_filter.retry = 0;
	ms_filter.state = 2;
	ms_filter.size = sizeof( ms_state_table ) / sizeof( uint8_t );
	ms_filter.table = ms_state_table;

	// Enable pullup on clock
	cbi( MS_CLOCK_DDR, MS_DATA_BIT );
	cbi( MS_CLOCK_DDR , MS_CLOCK_BIT );
	sbi( MS_CLOCK_PORT, MS_CLOCK_BIT );

	// Set initial resolution, frequency, and scale
	SetMouseDefault( );

	// Set mouse interrupts
	ms_clock_falling_edge( );
	ms_clock_clear_interrupt( );
	ms_clock_enable_interrupt( );

	// Enable timer interrupt (1 per ms)
	TimerInit( );
}

/*
 ********************************************************************************
 * MS_EventTask
 *
 * This task is run by the LUFA scheduler as part of the round robin algorithm.
 * It polls the mouse buffer for data and passes it to the EventFilter
 * for processing. It also monitors the timeout trigger. If the trigger has been
 * tripped then the mouse hasn't sent an update during the previous sample period.
 * The analog controls are reset to center values.
 ********************************************************************************
 */
void MS_EventTask( void )
{
	while( HasScancode( ) )
		EventFilter( GetScancode( ) );

	if ( ms_trigger.action == 1 )
	{
		ms_timer_disable_interrupt( );
		ms_trigger.action = 0;
		ms_timer_enable_interrupt( );
		
		usb_joystick_move_zrz(ACTION_VW_IDLE, ACTION_VW_IDLE);

		usb_joystick_press_dir(NO_BUTTON);

		usb_joystick_release(R2_BUTTON);
		usb_joystick_release(L2_BUTTON);

		usb_joystick_send();
	}
}

/*
 ********************************************************************************
 * EventFilter
 *
 * Mouse events are handled by the state machine after hard and soft resets.
 * During a reset sequence the mouse is brought through a series of commands that
 * enable the scroll wheel, set the resolution, frequency, and scale, and finally
 * ends with 0xF4 (Enable Data Reporting). The mouse responds with "acknowledge"
 * (0xFA) then enables data reporting and resets its movement counters.
 *
 * In stream mode the mouse sends movement data when it detects movement or a change
 * in state of one or more mouse buttons. The maximum rate at which this data may
 * be reported is known as the sample rate. This parameter ranges from 10 - 200
 * samples/sec, with a default value of 100 samples/sec. The host may set this value
 * using the "Set Sample Rate" (0xF3) command.
 *
 * Note that reporting is disabled by default. The mouse will not actually issue any
 * movement data packets until it receives the "Enable Data Reporting" (0xF4) command. 
 * Stream mode is the default operating mode, and is otherwise set using the "Set
 * Stream Mode" (0xEA) command. 
 *
 * The event filter expects the mouse to use the following 4-byte movement data
 * packet: 
 *		   --------------------------------------------------------------------
 *		  |Bit 7	  |Bit 6	  |Bit 5  |Bit 4  |Bit 3 |Bit 2	 |Bit 1 |Bit 0 |
 *		   --------------------------------------------------------------------
 * Byte 1 |Y overflow |X overflow |Y sign |X sign |1	 |Middle |Right |Left  |
 *		   --------------------------------------------------------------------
 * Byte 2 |							   X movement							   |
 *		   --------------------------------------------------------------------
 * Byte 3 |							   Y movement							   |
 *		   --------------------------------------------------------------------
 * Byte 4 |							   Z movement							   |
 *		   --------------------------------------------------------------------
 ********************************************************************************
 */
static void EventFilter( uint8_t scancode )
{
	if( ms_filter.state < ms_filter.size )
	{
		uint8_t target = pgm_read_byte( &ms_filter.table[ms_filter.state] ); 
		if( ( scancode == target ) || ( target == 0xFA && scancode == 0xF4 ) )
		{
			uint8_t byte = pgm_read_byte( &ms_filter.table[ms_filter.state + 1] );
			if( byte == 0xD1 )
				SendByte( ms_config.resolution );
			else if( byte == 0xD2 )
				SendByte( ( ms_config.scale == 1 )? 0xE6 : 0xE7 );
			else if( byte == 0xD3 )
				SendByte( ms_config.frequency );
			else if ( byte > 0x00 )
				SendByte( byte );
			ms_filter.state += 2;
		}
		else if ( ms_filter.retry++ < 5 )
		{
			ResetState( );
		}
		else
		{
			KB_ErrorStateLed( );
			ms_filter.state = 0xFF;
			ms_filter.retry = 0;
		}
	}

	// Bit 4 of the command byte should always be set. If it is not then something is wrong.
	// Ignore the sample. Hopefully this will eventually get the filter in synch with
	// the device.
	else if ( ms_packet.size == 0 && ( scancode & 0x08 ) == 0 )
	{
		ms_packet.size = 0;
	}

	// Gather a four byte mouse vector. Forward the bytes to the event handler where button
	// presses are mapped into actions. The X and Y vectors are translated into wiper values
	// and sent to the MCP42010 digital potentiometers.
	else
	{
		ms_filter.retry = 0;
		ms_packet.bytes[ms_packet.size++] = scancode;
		if( ms_packet.size == sizeof( ms_packet.bytes ) )
		{
			EventDispatcher( &ms_packet );
			ms_packet.size = 0;
		}
	}
}

static const uint8_t weapons[4] = {LEFT_BUTTON, DOWN_BUTTON, UP_BUTTON, RIGHT_BUTTON};
static uint8_t selected = 0;

typedef struct
{
	uint8_t pos;
#ifdef FARCRY2
	uint8_t pos2;
	double  delay;
#endif
} s_move; 

static void Val(vector_t *vector, s_move* move, double mul)
{
	uint16_t val;

#ifdef FARCRY2
	val = 0.2*pow(vector->data, 2); //[0..127] -> [0..645]

	if(val > 645) val = 645;

	move->delay = 0;
#else
	val = mul*vector->data;
#endif
	
	if(val) val += ACTION_VW_DEAD_ZONE;
	
	if(val <= ACTION_VW_MAXVECTOR)
	{
		if(vector->sign)
		{
			move->pos = ACTION_VW_IDLE - val;
		}
		else
		{
			move->pos = ACTION_VW_IDLE + val;
		}
	}
	else
	{
		if(vector->sign)
		{
			move->pos = 0;
#ifdef FARCRY2
			move->pos2 = 1;
#endif
		}
		else
		{
			move->pos = 255;
#ifdef FARCRY2
			move->pos2 = 254;
#endif
		}
#ifdef FARCRY2
		move->delay = 10*vector->data/127;
#endif
	}
}

/*
 ********************************************************************************
 * EventDispatcher
 ********************************************************************************
 */
static void EventDispatcher( packet_t *packet )
{
	vector_t vx, vy;
	uint8_t zsign, z;
	s_move mz, mrz;

	VectorX( packet, &vx );	// Extract X vector from the mouse packet.
	VectorY( packet, &vy );	// Extract Y vector from the mouse packet.
	
	Val(&vx, &mz, 3);
	Val(&vy, &mrz, 3);
	
	mrz.pos = 0xFF - mrz.pos;

#ifdef FARCRY2
	mrz.pos2 = 0xFF - mrz.pos2;
#endif
	
	usb_joystick_move_zrz(mz.pos, mrz.pos);

	if( ! ( packet->left == ms_dispatch.left ) )
	{
		ms_dispatch.left = packet->left;
		if(ms_dispatch.left)
			usb_joystick_press(R1_BUTTON);
		else
			usb_joystick_release(R1_BUTTON);
	}

	if( ! ( packet->right == ms_dispatch.right ) )
	{
		ms_dispatch.right = packet->right;
		if(ms_dispatch.right)
			usb_joystick_press(L1_BUTTON);
		else
			usb_joystick_release(L1_BUTTON);
	}

	if( ! ( packet->middle == ms_dispatch.middle ) )
	{
		ms_dispatch.middle = packet->middle;
		if(ms_dispatch.middle)
			usb_joystick_press(RIGHT_HAT_BUTTON);
		else
			usb_joystick_release(RIGHT_HAT_BUTTON);
	}

	zsign = packet->z > 128;
	z = ( zsign )? ~packet->z + 1 : packet->z;
	
	if(z > 0)
	{
		if( zsign == 0 )
		{
			if( z >= 1 )
			{
				usb_joystick_press(R2_BUTTON);
			}
		}
		else
		{
			if( z >= 1 )
			{
				usb_joystick_press(L2_BUTTON);
			}
		}
	}
	else
	{
		usb_joystick_release(R2_BUTTON);
		usb_joystick_release(L2_BUTTON);
	}
	
	usb_joystick_send();
	
#ifdef FARCRY2
	if(mz.delay && !mrz.delay)
	{
		LED_ON;
		_delay_ms(mz.delay);
		LED_OFF;
		usb_joystick_move_zrz(mz.pos2, mrz.pos);
		usb_joystick_send();
	}
	
	if(!mz.delay && mrz.delay)
	{
		LED_ON;
		_delay_ms(mrz.delay);
		LED_OFF;
		usb_joystick_move_zrz(mz.pos, mrz.pos2);
		usb_joystick_send();
	}
	
	if(mz.delay && mrz.delay)
	{
		if(mz.delay > mrz.delay)
		{
			LED_ON;
			usb_joystick_move_zrz(mz.pos, mrz.pos);
			usb_joystick_send();
			
			_delay_ms(mrz.delay);
			
			usb_joystick_move_zrz(mz.pos, mrz.pos2);
			usb_joystick_send();
			
			_delay_ms(mz.delay-mrz.delay);			
			LED_OFF;
			usb_joystick_move_zrz(mz.pos2, mrz.pos2);
			usb_joystick_send();
		}
		else
		{
			LED_ON;
			usb_joystick_move_zrz(mz.pos, mrz.pos);
			usb_joystick_send();
			
			_delay_ms(mz.delay);
			
			usb_joystick_move_zrz(mz.pos2, mrz.pos);
			usb_joystick_send();
			
			_delay_ms(mrz.delay-mz.delay);
			LED_OFF;
			usb_joystick_move_zrz(mz.pos2, mrz.pos2);
			usb_joystick_send();
		}
	}
#endif
}

/*
 ********************************************************************************
 * VectorX
 ********************************************************************************
 */
vector_t *VectorX( packet_t *packet, vector_t *vector )
{
	vector->label = 'X' - 64;
	vector->sign = packet->xsign;
	vector->overflow = packet->xoverflow;
	vector->data = packet->x;

	return AbsoluteValue( vector );
}

/*
 ********************************************************************************
 * VectorY
 ********************************************************************************
 */
vector_t *VectorY( packet_t *packet, vector_t *vector )
{
	vector->label = 'Y' - 64;
	vector->sign = packet->ysign;
	vector->overflow = packet->yoverflow;
	vector->data = packet->y;

	return AbsoluteValue( vector );
}

/*
 ********************************************************************************
 * AbsoluteValue
 ********************************************************************************
 */
static vector_t *AbsoluteValue( vector_t *vector )
{
	// Need absolute value. Two's complement negative values.
	if( vector->sign )
		vector->data = ~vector->data + 1;

	// Overflow? We don't want no stinken overflow!
	if( vector->overflow )
		vector->data = ACTION_VW_MAXVECTOR;

	return vector;
}

/*
 ********************************************************************************
 * SetMouseDefault
 *
 * The default mouse settings are used for Power-on-reset and Soft reset
 * functions.
 ********************************************************************************
 */
static void SetMouseDefault( void )
{
	ms_config.resolution = 3;
	ms_config.frequency = 100;
	ms_config.scale = 1;
}

/*
 ********************************************************************************
 * MS_MouseReset
 *
 * Sets default resolution, sample frequency, and scale then does a soft reset.
 ********************************************************************************
 */
void MS_MouseReset( void )
{
	SetMouseDefault( );
	ResetState( );
}
 
/*
 ********************************************************************************
 * ResetState
 *
 * The mouse responds to the 0xFF (Reset) command with "acknowledge" (0xFA) then
 * enters reset mode. The MS_EventFilter is called by MS_EventTask whenever
 * MS_ClockInterrupt puts a byte in the receive buffer. During system startup, or
 * when a Reset command is sent to the mouse, the MS_EventFilter runs the mouse
 * through a startup sequence. The sequence, coded as a state machine, enables the
 * scroll wheel and set the resolution, sample frequency, and scale.
 ********************************************************************************
 */
static void ResetState( void )
{
	SendByte( 0xFF );
	ms_filter.state = 0;
}

/*
 ********************************************************************************
 * MS_SetResolution
 * 
 * The standard mouse has two counters that keep track of movement: the X movement
 * counter and the Y movement counter. These are 9-bit 2's complement values and
 * each has an associated overflow flag. Their contents, along with the state of
 * the three mouse buttons, are sent to the host in the form of a 3-byte movement
 * data packet. The movement counters represent the mouse's offset relative to its
 * position when the previous movement data packet was issued, or when the last
 * non-"Resend" (0xFE) command was successfully sent to the host. 
 *
 * When the mouse reads its inputs it records the current state of its buttons and
 * increments / decrements the movement counters according to the amount of movement
 * that has occurred since the last input sample. If either of the counters has
 * overflowed, the appropriate overflow flag is set. Futher modification of the
 * counter is disabled until it the counters are reset (due to a packet being sent). 
 *
 * This parameter determines the amount by which the movement counters are
 * incremented / decremented. The default resolution is 4 counts/mm and the host
 * may change that value using the "Set Resolution" (0xE8) command. 
 *
 * 0xE8 (Set Resolution) - The mouse responds with "acknowledge" (0xFA) then reads
 * one byte from the host and again responds with "acknowledge" (0xFA) then resets
 * its movement counters. The byte read from the host determines the resolution as
 * follows: 
 *
 *		Byte Read
 *		From Host	Resolution	 
 *		00			1 count/mm  
 *		01			2 count/mm  
 *		02			4 count/mm  
 *		03			8 count/mm 
 ********************************************************************************
 */
void MS_SetResolution( uint8_t resolution )
{
	ms_config.resolution = resolution;
	ResetState( );
}

/*
 ********************************************************************************
 * MS_SetFrequency
 *
 * In stream mode the mouse sends movement data when it detects movement or a
 * change in state of one or more mouse buttons. The maximum rate at which this
 * data may be reported is known as the sample rate (i.e., frequency). This
 * parameter ranges from 10 - 200 samples/sec, with a default value of 100
 * samples/sec. The host may set this value using the "Set Sample Rate" (0xF3)
 * command. Note that reporting is disabled by default. The mouse will not
 * actually issue any movement data packets until it receives the "Enable Data
 * Reporting" (0xF4) command.
 *
 * 0xF3 (Set Sample Rate) - The mouse responds with "acknowledge" (0xFA) then
 * reads one more byte from the host. The mouse saves this byte as the new
 * sample rate. After receiving the sample rate, the mouse again responds with
 * "acknowledge" (0xFA) and resets its movement counters. Valid sample rates
 * are 10, 20, 40, 60, 80, 100, and 200 samples/sec. 
 ********************************************************************************
 */
void MS_SetFrequency( uint8_t frequency )
{
	ms_config.frequency = frequency;
	ResetState( );
}

/*
 ********************************************************************************
 * MS_SetScaling
 *
 * Scaling = 1 if scaling is 2:1; 0 if scaling is 1:1 (commands 0xE7 and 0xE6). 
 * 0xE7 (Set Scaling 2:1) - The mouse responds with "acknowledge" (0xFA) then
 *				enables 2:1 scaling. 
 * 0xE6 (Set Scaling 1:1) - The mouse responds with "acknowledge" (0xFA) then
 *				enables 1:1 scaling. 
 *
 * This parameter that does not affect the movement counters, but does affect the
 * reported value of these counters. By default, the mouse uses 1:1 scaling, which
 * has no effect on the reported mouse movement. However, the host may select 2:1
 * scaling by issuing the "Set Scaling 2:1" (0xE7) command. If 2:1 scaling is
 * enabled, the mouse will apply the following algorithm to the movement counters
 * before sending their contents to the host: 
 *
 *		Movement	Reported
 *		Counter		Movement  
 *		0			0
 *		1			1
 *		2			1
 *		3			3
 *		4			6
 *		5			9
 *		N > 5		2 * N
 ********************************************************************************
 */
void MS_SetScaling( uint8_t scale )
{
	ms_config.scale = scale;
	ResetState( );
}

/*
 ********************************************************************************
 * GetScancode
 *
 * Fetch a byte from the receive buffer. Returns 0 if the buffer is empty. It's
 * not a good idea to call this routine when the buffer is empty because there
 * is no sentinal value. All byte values from 0 - 255 are valid. Poll for data
 * using HasScancode before calling this function.
 ********************************************************************************
 */

static uint8_t GetScancode( void )
{
	return RING_GetElement( &ms_driver.ringbuffer );
}

/*
 ********************************************************************************
 * HasScancode
 *
 * Returns true if there is data in the receive buffer.
 ********************************************************************************
 */
static uint8_t HasScancode( void )
{
	return RING_HasElement( &ms_driver.ringbuffer );
}

 /*
 ********************************************************************************
 * SendByte
 *
 * The mouse implements a bi-directional protocol. The mouse can send data
 * to the Host and the Host can send data to the mouse. The Host has the
 * ultimate control over direction. It can at anytime send a command to the mouse.
 * 
 * The mouse is free to send data to the host when both the Data and Clock lines
 * are high (Idle). If the host takes the Clock line low, the mouse will buffer
 * any data until the Clock is released, ie goes high. Should the Host take the
 * Data line low, then the mouse will prepare to accept a command from the host.
 *
 * http://www.computer-engineering.org/ps2protocol/
 *
 *	1) Bring the Clock line low for at least 100 microseconds. 
 *	2) Bring the Data line low. 
 *	3) Release the Clock line. 
 *	4) Wait for the device to bring the Clock line low. 
 *	5) Set/reset the Data line to send the first data bit 
 *	6) Wait for the device to bring Clock high. 
 *	7) Wait for the device to bring Clock low. 
 *	8) Repeat steps 5-7 for the other seven data bits and the parity bit 
 *	9) Release the Data line. 
 * 10) Wait for the device to bring Data low. 
 * 11) Wait for the device to bring Clock  low. 
 * 12) Wait for the device to release Data and Clock
 ********************************************************************************
 */

static void SendByte( uint8_t data )
{
	uint8_t ms_mask = 1;

	ms_clock_disable_interrupt( );
	ms_timer_disable_interrupt( );
	
	// Clear the ISR state variables
	ms_driver.buffer = 0;
	ms_driver.error = 0;
	ms_driver.bit_n = 1;

	// 1. Bring the Clock line low for at least 100 microseconds. 
	cbi( MS_CLOCK_PORT, MS_CLOCK_BIT );
	sbi( MS_CLOCK_DDR, MS_CLOCK_BIT );
	_delay_us( 100 );

	// 2. Bring the Data line low.
	cbi( MS_DATA_PORT, MS_DATA_BIT );
	sbi( MS_DATA_DDR, MS_DATA_BIT );

	// 3. Release the Clock line.
	cbi( MS_CLOCK_DDR, MS_CLOCK_BIT );
	sbi( MS_CLOCK_PORT, MS_CLOCK_BIT );
	loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// 4. Wait for the device to bring the Clock line low. 
	loop_until_bit_is_clear( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// 5. Set/Reset the Data line to send data bits 
	do {
		if( data & ms_mask )
			cbi( MS_DATA_DDR, MS_DATA_BIT );	// Take the data line high.
		else
			sbi( MS_DATA_DDR, MS_DATA_BIT );	// Take the data line low.

		// 6. Wait for the device to bring Clock high. 
		loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );

		// 7. Wait for the device to bring Clock low.
		loop_until_bit_is_clear( MS_CLOCK_PIN, MS_CLOCK_BIT );

		// 8A. Repeat steps 5-7 for the other seven data bits
	} while( ms_mask <<= 1 );

	// 8B. Repeat steps 5-7 for the parity bit.
	if( parity_even_bit( data ) )
		sbi( MS_DATA_DDR, MS_DATA_BIT );  // Take the data line low.
	else
		cbi( MS_DATA_DDR, MS_DATA_BIT );  // Take the data line high.
	loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );
	loop_until_bit_is_clear( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// 9. Release the data line. Make data line an input. Disable pull-up.
	cbi( MS_DATA_DDR, MS_DATA_BIT );
	cbi( MS_DATA_PORT, MS_DATA_BIT );
	loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// 10. Wait for the device to bring Data low.
	// ACK from Mouse.
	loop_until_bit_is_clear( MS_DATA_PIN, MS_DATA_BIT );

	// 11. Wait for the device to bring Clock low.
	loop_until_bit_is_clear( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// Wait for the device to release Data and Clock.
	// Bus is idle.
	loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// It is critical to enable interrupts quickly after
	// detecting idle condition otherwise first bit of
	// ACK may be lost.
	ms_clock_clear_interrupt( );
	ms_clock_enable_interrupt( );
	ms_timer_enable_interrupt( );
}

/*
 ********************************************************************************
 * MS_ClockInterrupt
 *
 * The transmission of data in the forward direction, ie mouse to Host is
 * done with a frame of 11 bits. The first bit is a Start Bit (Logic 0) followed
 * by 8 data bits (LSB First), one Parity Bit (Odd Parity) and a Stop Bit (Logic 1).
 * Each bit should be read on the falling edge of the clock.
 *		bit 1: start bit (low)
 * bits 2 - 9: data bits (least significant bit first)
 *		bit 10: parity bit (odd parity)
 *		bit 11: stop bit (high)
 ********************************************************************************
 */
ISR( MS_ClockInterrupt )
{
	// Reload the mouse timeout and clear trigger.
	ms_trigger.timer = ms_trigger.timeout;
	ms_trigger.action = 0;

	// Start bit must be low.
	if( ms_driver.bit_n == 1 && bit_is_set( MS_DATA_PIN, MS_DATA_BIT ) )
		goto abort;

	// Read data bits and calculate parity.
	else if( ms_driver.bit_n >= 2 && ms_driver.bit_n <= 9 )
	{
		if( bit_is_set( MS_DATA_PIN, MS_DATA_BIT ) )
			ms_driver.buffer |= ( 1 << ( ms_driver.bit_n - 2 ) );
	}

	// Check parity. Ask for re-send if this byte is corrupt.
	else if( ms_driver.bit_n == 10 )
	{
		if( bit_is_set( MS_DATA_PIN, MS_DATA_BIT ) == parity_even_bit( ms_driver.buffer ) )
		{
			if( ms_driver.error == 0 )
				SendByte( 0xFE );
			ms_driver.error = 1;
			goto abort;
		}
	}

	// Check for stop bit high. If ok then queue byte.
	else if( ms_driver.bit_n == 11 )
	{
		if( bit_is_clear( MS_DATA_PIN, MS_DATA_BIT ) )
		{
			if( ms_driver.error == 0 )
				SendByte( 0xFE );
			ms_driver.error = 1;
			goto abort;
		}
		else
		{
			RING_AddElement( &ms_driver.ringbuffer, ms_driver.buffer );
		}
		ms_driver.buffer = 0;
		ms_driver.bit_n = 0;
		ms_driver.error = 0;
	}
	ms_driver.bit_n++;

abort:
	asm volatile ( "nop":: );
}

/*
 ********************************************************************************
 * TimerInit
 *
 * 16-Bit milli-second timer interrupt. Causes MS_TimerInterrupt to be called
 * once per milli-second.
 ********************************************************************************
 */
static void TimerInit( void )
{
  // Set TCNT1 Timer for 1 mS
  OCR1A = 250;						// Counter reaches 250 every 1 mS (250,000 / 250 = 1000 Hz).
  TCCR1A =	( 0 << COM1A1 ) |		// Normal port operation. Output A compare pin disabled.
			( 0 << COM1A0 ) |
			( 0 << COM1B1 ) |		// Normal port operation. Output B compare pin disabled.
			( 0 << COM1B0 ) |
			( 0 << COM1C1 ) |		// Normal port operation. Output B compare pin disabled.
			( 0 << COM1C0 ) |
			( 0 << WGM11 ) |		// CTC, TOP = OCR1A, Update OCR1A Immediate, TOV1 = MAX
			( 0 << WGM10 );
  TCCR1B =	( 0 << ICNC1 ) |		// De-activate Input Capture Noise Canceller
			( 0 << ICES1 ) |		// Input Capture Edge Select (Don't Care)
			( 0 << WGM13 ) |		// CTC, TOP = OCR1A, Update OCR1A Immediate, TOV1 = MAX
			( 1 << WGM12 ) |
			( 0 << CS12 ) |			// ClkIO / 64 (pre-scaler). 16,000,000 MHz / 64 = 250,000 Hz.
			( 1 << CS11 ) |
			( 1 << CS10 );
	cbi( PRR0, PRTIM1 );			// Enable 16bit Timer / Counter1 module.
	ms_timer_enable_interrupt( );	// Timer/Counter Output Compare A Match interrupt enabled.
}

/*
 ********************************************************************************
 * MS_TimerInterrupt
 *
 * Decrement ms_timer to zero. At the transition from 1 to 0 ms_trigger is set to
 * 1 (true). The trigger is polled and cleared in the MS_EventTask handler. The
 * trigger is set when the user stops moving the mouse. The ms_timer is reset
 * during system initialization and whenever the MS_ClockInterrupt recieves
 * mouse data.
 ********************************************************************************
 */
ISR( MS_TimerInterrupt )
{
	if( ms_trigger.timer > 0 )
	{
		ms_trigger.timer--;
		if( ms_trigger.timer == 0 && ms_trigger.action == 0 )
		{
			ms_trigger.action = 1;
		}
	}
}

