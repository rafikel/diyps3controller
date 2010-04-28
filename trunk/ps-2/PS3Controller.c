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
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "usb_joystick.h"
#include "RingBuffer.h"
#include "KeyboardDriver.h"
#include "MouseDriver.h"

#include "led.h"

/*
 ********************************************************************************
 * Local Functions and Variables
 ********************************************************************************
 */
#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

/*
 ********************************************************************************
 * main
 ********************************************************************************
 */
int main( void )
{


    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    CPU_PRESCALE(0);
    LED_CONFIG;
    LED_ON;
    
    /* Initialize Keyboard Driver */
    KB_Init();

    /* Initialize Mouse Driver */
    MS_Init();
    
    /* Initialize USB Joystick variables */
    usb_joystick_raz();

    /* Enable interrupts */
    sei( );

    /* Initialize USB Joystick */
    usb_init();
    while (!usb_configured()) ;
    _delay_ms(1000);
    LED_OFF;

    /* Send a first packet */
    usb_joystick_send();

    /* Reset the mouse */
    MS_MouseReset();

    /*
	 * Comment that code to test rotation speed for a given position.
	 */
    while( 1 )
    {
        KB_EventTask();
		MS_EventTask();
    }
    
    /*
     * Uncomment that code to test rotation speed for a given position.
     */
//	while( 1 )
//    {
//		usb_joystick_move_zrz(153, 128);
//		usb_joystick_send();
//		_delay_ms(5000);
//	}

    return 0;
}

