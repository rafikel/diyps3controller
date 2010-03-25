/*
 ********************************************************************************
 * Copyright (c) 2009 Richard Burke
 * Copyright (c) 2010 Mathieu Laurendeau
 * All rights reserved.
 *
 * This file contains the main method.
 * 	- It initializes the AVR controller,
 * 		the mouse and keyboard drivers,
 * 		and the usb hid joystick layer.
 * 	- It polls ring buffers for new data.
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
 ********************************************************************************
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

