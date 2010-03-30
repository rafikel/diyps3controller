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
#include "System.h"

#include "led.h"

/*
 ********************************************************************************
 * Local Functions and Variables
 ********************************************************************************
 */
void main( void ) __attribute__ ((noreturn));

system_t Configuration;

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

#define TEST 0

/*
 ********************************************************************************
 * main
 ********************************************************************************
 */
void main( void )
{


    /* Disable watchdog if enabled by bootloader/fuses */
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    /* Disable clock division */
    CPU_PRESCALE(0);
    LED_CONFIG;
    LED_ON;
    
    /* Initialize Keyboard Driver */
    if(!TEST) KB_Init();

    /* Initialize Mouse Driver */
    if(!TEST) MS_Init();
    
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
	
	if(TEST)
	{
		usb_joystick_press(CROSS_BUTTON);
	}

    while( 1 )
    {
        if(!TEST)
		{
			KB_EventTask();
			MS_EventTask();
		}
		else
		{

			//That code is used to test rotation speed for a given position.

			usb_joystick_move_zrz(153, 128);
			usb_joystick_send();

			_delay_ms(5000);

			/*
			int i;

			usb_joystick_move_zrz(128, 83);
			usb_joystick_send();
			
			for(i=0 ; i ; i++)
			{
				_delay_ms(1000);
			}
			
			usb_joystick_move_zrz(128, 128);
			usb_joystick_send();
			
			LED_ON;
			_delay_ms(5000);
			LED_OFF;
			
			usb_joystick_move_zrz(128, 173);
			usb_joystick_send();
			
			for(i=0 ; i<1600 ; i++)
			{
				_delay_ms(1000);
			}
			
			usb_joystick_move_zrz(128, 128);
			usb_joystick_send();
			
			LED_ON;
			_delay_ms(5000);
			LED_OFF;
			*/
		}
    }
}

