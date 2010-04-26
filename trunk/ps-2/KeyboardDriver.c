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
#include <util/delay.h>
#include <util/parity.h>
#include "Libdefs.h"
#include "RingBuffer.h"
#include "KeyboardDriver.h"
#include "MouseDriver.h"
#include "EventAction.h"

#include "usb_joystick.h"
#include "led.h"

/*
 ********************************************************************************
 * Local Variables
 ********************************************************************************
 */
static volatile uint8_t kb_bit_n;
static volatile uint8_t kb_buffer;
static volatile uint8_t kb_error;

static ringbuff_t kb_ringbuffer;
static uint8_t kb_data[KB_BUFSIZE];

static volatile uint8_t kb_e0_f0;
static volatile uint16_t kb_scancode;

static uint8_t kb_lastbyte;
static uint8_t kb_retry;
static uint8_t kb_leds;
static uint8_t kb_cmd;

static uint16_t kb_led_dutycycle;
static uint16_t kb_led_delay;
static uint16_t kb_led_keycode;

/*
 ********************************************************************************
 * Private Functions
 ********************************************************************************
 */
static uint16_t KB_GetScancode( void );
static void KB_Send( uint8_t data );
static void KB_KeyboardEventFilter( uint16_t scancode );

/*
 ********************************************************************************
 * KB_Init
 ********************************************************************************
 */
void KB_Init( void )
{
	kb_bit_n = 1;
	kb_buffer = 0;
	kb_error = 0;

	kb_e0_f0 = 0;
	kb_scancode = 0;

	RING_Initialize( &kb_ringbuffer, kb_data, sizeof( kb_data ) );

	kb_lastbyte = 0;
	kb_retry = 0;
	kb_leds = 0;
	kb_cmd = 0;

	// Enable pullup on clock
	sbi( KB_CLOCK_PORT, KB_CLOCK_BIT );

	// Enable heartbeat
	KB_NormalStateLed( );

	// Set interrupts
	kb_clock_falling_edge( );
	kb_clock_enable_interrupt( );
}

/*
 ********************************************************************************
 * KB_EventTask
 ********************************************************************************
 */

void KB_EventTask( void )
{
	uint16_t scancode;

	if( ( scancode = KB_GetScancode( ) ) > 0 ) {
		KB_KeyboardEventFilter( scancode );
	}
	else if ( kb_led_delay-- == 0 )
	{
		KB_KeyboardEventFilter( kb_led_keycode );
		kb_led_delay = kb_led_dutycycle;
	}
}

/*
 ********************************************************************************
 * KB_ErrorStateLed
 ********************************************************************************
 */
void KB_ErrorStateLed( void )
{
	kb_led_keycode = KB_KeyPress( ScrollLockKey );
	kb_led_dutycycle = KB_LED_ERROR;
}

/*
 ********************************************************************************
 * KB_NormalStateLed
 ********************************************************************************
 */
void KB_NormalStateLed( void )
{
	kb_led_keycode = KB_KeyPress( ScrollLockKey );
	kb_led_dutycycle = KB_LED_NORMAL;
}

/*
 ********************************************************************************
 * KB_KeyboardEventFilter
 ********************************************************************************
 */
static void KB_KeyboardEventFilter( uint16_t scancode )
{
	// M
	if( scancode == KB_KeyPress( MKey ) )
	{
		KB_NormalStateLed( );
		MS_MouseReset( );
	}

	// SCROLL Lock
	else if ( scancode == KB_KeyPress( ScrollLockKey ) )
	{
		KB_Send( kb_lastbyte = 0xED );
		kb_cmd = 0x01;
		kb_retry = 0;
	}

	// NUM Lock
	else if( scancode == KB_KeyPress( NumLockKey ) )
	{
		KB_Send( kb_lastbyte = 0xED );
		kb_cmd = 0x02;
		kb_retry = 0;
	}

	// CAPS Lock
	else if ( scancode == KB_KeyPress( CapsLockKey ) )
	{
		KB_Send( kb_lastbyte = 0xED );
		kb_cmd = 0x04;
		kb_retry = 0;
	}

	// LED On/Off
	else if( ( scancode == KB_KeyPress( 0xFA ) ) && ( kb_lastbyte == 0xED ) )
	{
		kb_leds ^= kb_cmd;
		KB_Send( kb_lastbyte = kb_leds );
		kb_retry = 0;
	}

	// RETRY
	else if( scancode == KB_KeyPress( 0xFE ) )
	{
		if( kb_retry++ < 4 )
		{
			KB_Send( kb_lastbyte );
		}
		else if( kb_retry++ < 8 )
		{
			KB_Send( kb_lastbyte = 0xFF );
		}
	}

	// XBox Translation
	/*if( ! ( scancode == KB_KeyPress( 0xFA ) || scancode == KB_KeyPress( 0xFE ) || scancode == KB_KeyPress( ScrollLockKey ) || scancode == KB_KeyRelease( ScrollLockKey ) ) )
	{
		//LogWord( 'K', scancode );
		//EVA_InvokeEventAction( scancode );
	}*/
        switch( scancode )
        {
//sauter
            case KB_KeyPress( SpaceKey ):
                usb_joystick_press(CROSS_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( SpaceKey ):
                usb_joystick_release(CROSS_BUTTON);
                usb_joystick_send();
                break;
//s'accroupir
            case KB_KeyPress( RightControlKey ):
                usb_joystick_press(CIRCLE_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( RightControlKey ):
                usb_joystick_release(CIRCLE_BUTTON);
                usb_joystick_send();
                break;
//utiliser
            case KB_KeyPress( EKey ):
                usb_joystick_press(TRIANGLE_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( EKey ):
                usb_joystick_release(TRIANGLE_BUTTON);
                usb_joystick_send();
                break;
//recharger
            case KB_KeyPress( RKey ):
                usb_joystick_press(SQUARE_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( RKey ):
                usb_joystick_release(SQUARE_BUTTON);
                usb_joystick_send();
                break;
//gauche
            case KB_KeyPress( AKey ):
                usb_joystick_move_x(0x00);
                usb_joystick_send();
                break;
            case KB_KeyRelease( AKey ):
                usb_joystick_move_x(0x80);
                usb_joystick_send();
                break;
//droite
            case KB_KeyPress( DKey ):
                usb_joystick_move_x(0xFF);
                usb_joystick_send();
                break;
            case KB_KeyRelease( DKey ):
                usb_joystick_move_x(0x80);
                usb_joystick_send();
                break;
//avant
            case KB_KeyPress( WKey ):
                usb_joystick_move_y(0x00);
                usb_joystick_send();
                break;
            case KB_KeyRelease( WKey ):
                usb_joystick_move_y(0x80);
                usb_joystick_send();
                break;
//arrière
            case KB_KeyPress( SKey ):
                usb_joystick_move_y(0xFF);
                usb_joystick_send();
                break;
            case KB_KeyRelease( SKey ):
                usb_joystick_move_y(0x80);
                usb_joystick_send();
                break;
//courir
            case KB_KeyPress( LeftShiftKey ):
                usb_joystick_press(LEFT_HAT_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( LeftShiftKey ):
                usb_joystick_release(LEFT_HAT_BUTTON);
                usb_joystick_send();
                break;

//start
            case KB_KeyPress( EscapeKey ):
                usb_joystick_press(START_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( EscapeKey ):
                usb_joystick_release(START_BUTTON);
                usb_joystick_send();
                break;

//select
            case KB_KeyPress( TabKey ):
                usb_joystick_press(SELECT_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( TabKey ):
                usb_joystick_release(SELECT_BUTTON);
                usb_joystick_send();
                break;

//haut
            case KB_KeyPress( D1Key ):
                usb_joystick_press_dir(UP_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( D1Key ):
                usb_joystick_press_dir(NO_BUTTON);
                usb_joystick_send();
                break;

//droite
            case KB_KeyPress( D2Key ):
                usb_joystick_press_dir(RIGHT_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( D2Key ):
                usb_joystick_press_dir(NO_BUTTON);
                usb_joystick_send();
                break;

//gauche
            case KB_KeyPress( D3Key ):
                usb_joystick_press_dir(LEFT_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( D3Key ):
                usb_joystick_press_dir(NO_BUTTON);
                usb_joystick_send();
                break;

//bas
            case KB_KeyPress( D4Key ):
                usb_joystick_press_dir(DOWN_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( D4Key ):
                usb_joystick_press_dir(NO_BUTTON);
                usb_joystick_send();
                break;

//r2
            case KB_KeyPress( GKey ):
                usb_joystick_press(R2_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( GKey ):
                usb_joystick_release(R2_BUTTON);
                usb_joystick_send();
                break;

//rh
            case KB_KeyPress( FKey ):
                usb_joystick_press(RIGHT_HAT_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( FKey ):
                usb_joystick_release(RIGHT_HAT_BUTTON);
                usb_joystick_send();
                break;
//l2
            case KB_KeyPress( TKey ):
                usb_joystick_press(L2_BUTTON);
                usb_joystick_send();
                break;
            case KB_KeyRelease( TKey ):
                usb_joystick_release(L2_BUTTON);
                usb_joystick_send();
                break;
        }
        
}

/*
 ********************************************************************************
 * KB_GetScancode
 ********************************************************************************
 */
static uint16_t KB_GetScancode( void )
{
	uint8_t byte;

	kb_scancode = 0;
	if( RING_HasElement( &kb_ringbuffer ) )
	{
		switch( byte = RING_GetElement( &kb_ringbuffer ) )
		{
			case 0xE0:
				kb_e0_f0 = (1<<KB_E0);
				break;
			case 0xF0:
				kb_e0_f0 |= (1<<KB_F0);
				break;
			default:
				kb_scancode = EVA_NewEvent( EVA_NewEventKey( Keyboard, kb_e0_f0 ), EVA_NewEventValue( byte ) );
				kb_e0_f0 = 0;
		}
	}

	return kb_scancode;
}

/*
 ********************************************************************************
 * KB_Send
 *
 * The keyboard implements a bi-directional protocol. The keyboard can send data
 * to the Host and the Host can send data to the Keyboard. The Host has the
 * ultimate control over direction. It can at anytime (although the not
 * recommended) send a command to the keyboard.
 * 
 * The keyboard is free to send data to the host when both the KBD Data and KBD
 * Clock lines are high (Idle). The KBD Clock line can be used as a Clear to
 * Send line. If the host takes the KBD Clock line low, the keyboard will buffer
 * any data until the KBD Clock is released, ie goes high. Should the Host take
 * the KBD Data line low, then the keyboard will prepare to accept a command
 * from the host.
 *
 * http://www.computer-engineering.org/ps2protocol/
 *
 *  1) Bring the Clock line low for at least 100 microseconds. 
 *  2) Bring the Data line low. 
 *  3) Release the Clock line. 
 *  4) Wait for the device to bring the Clock line low. 
 *  5) Set/reset the Data line to send the first data bit 
 *  6) Wait for the device to bring Clock high. 
 *  7) Wait for the device to bring Clock low. 
 *  8) Repeat steps 5-7 for the other seven data bits and the parity bit 
 *  9) Release the Data line. 
 * 10) Wait for the device to bring Data low. 
 * 11) Wait for the device to bring Clock  low. 
 * 12) Wait for the device to release Data and Clock
 ********************************************************************************
 */
static void KB_Send( uint8_t data )
{
	uint8_t kb_mask = 1;

	kb_clock_disable_interrupt( );

	// Clear the ISR state variables
	kb_buffer = 0;
	kb_error = 0;
	kb_bit_n = 1;

	// 1. Bring the Clock line low for at least 100 microseconds. 
	cbi( KB_CLOCK_PORT, KB_CLOCK_BIT );
	sbi( KB_CLOCK_DDR, KB_CLOCK_BIT );
	_delay_us( 100 );

	// 2. Bring the Data line low.
	cbi( KB_DATA_PORT, KB_DATA_BIT );
	sbi( KB_DATA_DDR, KB_DATA_BIT );

	// 3. Release the Clock line.
	cbi( KB_CLOCK_DDR, KB_CLOCK_BIT );
	sbi( KB_CLOCK_PORT, KB_CLOCK_BIT );
	loop_until_bit_is_set( KB_CLOCK_PIN, KB_CLOCK_BIT );

	// 4. Wait for the device to bring the Clock line low. 
	loop_until_bit_is_clear( KB_CLOCK_PIN, KB_CLOCK_BIT );

	// 5. Set/Reset the Data line to send the first data bit 
	do {
		if( data & kb_mask )
			cbi( KB_DATA_DDR, KB_DATA_BIT );	// Take the data line high.
		else
			sbi( KB_DATA_DDR, KB_DATA_BIT );	// Take the data line low.

		// 6. Wait for the device to bring Clock high. 
		loop_until_bit_is_set( KB_CLOCK_PIN, KB_CLOCK_BIT );

		// 7. Wait for the device to bring Clock low.
		loop_until_bit_is_clear( KB_CLOCK_PIN, KB_CLOCK_BIT );

		// 8A. Repeat steps 5-7 for the other seven data bits
	} while( kb_mask <<= 1 );

	// 8B. Repeat steps 5-7 for the parity bit.
	if( parity_even_bit( data ) )
		sbi( KB_DATA_DDR, KB_DATA_BIT );	// Take the data line low.
	else
		cbi( KB_DATA_DDR, KB_DATA_BIT );	// Take the data line high.
	loop_until_bit_is_set( KB_CLOCK_PIN, KB_CLOCK_BIT );
	loop_until_bit_is_clear( KB_CLOCK_PIN, KB_CLOCK_BIT );

	// 9. Release the data line.
	cbi( KB_DATA_DDR, KB_DATA_BIT );
	cbi( KB_DATA_PORT, KB_DATA_BIT );
	loop_until_bit_is_set( MS_CLOCK_PIN, MS_CLOCK_BIT );

	// 10. Wait for the device to bring Data low.
	// ACK from Keyboard.
	loop_until_bit_is_clear( KB_DATA_PIN, KB_DATA_BIT );

	// 11. Wait for the device to bring Clock  low.
	loop_until_bit_is_clear( KB_CLOCK_PIN, KB_CLOCK_BIT );

	// Wait for the device to release Data and Clock.
	// Bus is idle.
	loop_until_bit_is_set( KB_CLOCK_PIN, KB_CLOCK_BIT );

	// It is critical to enable interrupts quickly after
	// detecting idle condition otherwise first bit of
	// ACK may be lost.
	kb_clock_clear_interrupt( );
	kb_clock_enable_interrupt( );
}

/*
 ********************************************************************************
 * KB_ClockInterrupt
 *
 * The transmission of data in the forward direction, ie Keyboard to Host is
 * done with a frame of 11 bits. The first bit is a Start Bit (Logic 0) followed
 * by 8 data bits (LSB First), one Parity Bit (Odd Parity) and a Stop Bit (Logic 1).
 * Each bit should be read on the falling edge of the clock.
 *      bit 1: start bit (low)
 * bits 2 - 9: data bits (least significant bit first)
 *     bit 10: parity bit (odd parity)
 *     bit 11: stop bit (high)
 ********************************************************************************
 */
ISR( KB_ClockInterrupt )
{
	// Start bit must be low.
	if( kb_bit_n == 1 && bit_is_set( KB_DATA_PIN, KB_DATA_BIT ) )
		goto abort;

	// Read data bits and calculate parity.
	else if( kb_bit_n >= 2 && kb_bit_n <= 9 )
	{
		if( bit_is_set( KB_DATA_PIN, KB_DATA_BIT ) )
			kb_buffer |= ( 1 << (kb_bit_n - 2 ) );
	}

	// Check parity. Ask for re-send if this byte is corrupt.
	else if( kb_bit_n == 10 )
	{
		if( bit_is_set( KB_DATA_PIN, KB_DATA_BIT )== parity_even_bit( kb_buffer ) )
		{
			if( kb_error == 0 )
				KB_Send( 0xFE );
			kb_error = 1;
			goto abort;
		}
	}

	// Check for stop bit high and queue byte if everything is ok.
	else if( kb_bit_n == 11 )
	{
		if( bit_is_clear( KB_DATA_PIN, KB_DATA_BIT ) )
		{
			if( kb_error == 0 )
				KB_Send( 0xFE );
			kb_error = 1;
			goto abort;
		}
		else
		{
			RING_AddElement( &kb_ringbuffer, kb_buffer );
		}
		kb_buffer = 0;
		kb_bit_n = 0;
		kb_error = 0;
	}
	kb_bit_n++;

abort:
	asm volatile ("nop"::);
}
