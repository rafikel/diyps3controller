/*
             LUFA Library
     Copyright (C) Dean Camera, 2010.
              
  dean [at] fourwalledcubicle [dot] com
      www.fourwalledcubicle.com
*/

/*
  Copyright 2010  Denver Gingerich (denver [at] ossguy [dot] com)
      Based on code by Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this 
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in 
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting 
  documentation, and that the name of the author not be used in 
  advertising or publicity pertaining to distribution of the 
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/** \file
 *
 *  Main source file for the Keyboard demo. This file contains the main tasks of the demo and
 *  is responsible for the initial application hardware configuration.
 */
 
#include "sixaxis_pair_emu.h"
#include <util/delay.h>

/*
 * The master bdaddr, read from eeprom at startup.
 */
uint8_t VolatileMasterBdaddr[6];

/*
 * Indicates if the master bdaddr was already requested or not.
 */
static unsigned char reply = 0;

/** Indicates what report mode the host has requested, true for normal HID reporting mode, false for special boot
 *  protocol reporting mode.
 */
bool UsingReportProtocol = true;

/** Current Idle period. This is set by the host via a Set Idle HID class request to silence the device's reports
 *  for either the entire idle duration, or until the report status changes (e.g. the user presses a key).
 */
uint16_t IdleCount = 500;

/** Current Idle period remaining. When the IdleCount value is set, this tracks the remaining number of idle
 *  milliseconds. This is separate to the IdleCount timer and is incremented and compared as the host may request 
 *  the current idle period via a Get Idle HID class request, thus its value must be preserved.
 */
uint16_t IdleMSRemaining = 0;

/*
 * The reference report data.
 */
static uint8_t report[49] = {
			0x01,0x00,0x00,0x00,
			0x00, //0x01 = PS3 button
			0x00,0x7a,0x80,0x82,
			0x7d,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,
			0x00,0x00,0x00,0x00,
			0x02,0xee,0x10,0x00,
			0x00,0x00,0x00,0x02,
			0xff,0x77,0x01,0x80,
			0x02,0x1f,0x02,0x02,
			0x01,0x9c,0x00,0x05
};

#define USART_BAUDRATE 500000
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

uint8_t* pdata = report;
unsigned char i = 0;

ISR(USART1_RX_vect)
{
  pdata[i] = UDR1; // Fetch the received byte value
  //UDR1 = pdata[i]; // Echo back the received byte back to the computer

  i++;
  i%=sizeof(report);
}

void serial_init(void)
{
   UBRR1H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
   UBRR1L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register

   UCSR1B |= (1 << RXEN1) | (1 << TXEN1); // Turn on the transmission and reception circuitry

   UCSR1C |= _BV(UCSZ10) | _BV(UCSZ11); /* 8 bits per char */

   UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)

   return;
}


#define LED_ON (PORTD |= (1<<6))
#define LED_OFF (PORTD &= ~(1<<6))

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{

	SetupHardware();
	
	for (;;)
	{
		HID_Task();
		USB_USBTask();
	}
}

/** Configures the board hardware and chip peripherals for the demo's functionality. */
void SetupHardware(void)
{
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

  serial_init();

	/* Hardware Initialization */
	//LEDs_Init();
  DDRD |= (1<<6);
  USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);

	/* Default to report protocol on connect */
	UsingReportProtocol = true;
}

/** Event handler for the USB_Disconnect event. This indicates that the device is no longer connected to a host via
 *  the status LEDs.
 */
void EVENT_USB_Device_Disconnect(void)
{
	/* Indicate USB not ready */
	LEDs_SetAllLEDs(LEDMASK_USB_NOTREADY);
}

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the keyboard device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{	
	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDMASK_USB_READY);

	/* Setup Keyboard Keycode Report Endpoint */
	if (!(Endpoint_ConfigureEndpoint(SIXAXIS_IN_EPNUM, EP_TYPE_INTERRUPT,
		                             ENDPOINT_DIR_IN, SIXAXIS_EPSIZE,
	                                 ENDPOINT_BANK_SINGLE)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	/* Setup Keyboard LED Report Endpoint */
	if (!(Endpoint_ConfigureEndpoint(SIXAXIS_OUT_EPNUM, EP_TYPE_INTERRUPT,
		                             ENDPOINT_DIR_OUT, SIXAXIS_EPSIZE,
	                                 ENDPOINT_BANK_SINGLE)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	USB_Device_EnableSOFEvents();
}

/*
 * For debug purposes!
static unsigned int nb_requests = 0;

USB_Request_Header_t log[64];
*/

static char last_set_data[64];

const char PROGMEM buf301[] = {
    0x00,
    0x01,0x03,0x00,0x04,0x0c,0x01,0x02,0x18,
    0x18,0x18,0x18,0x09,0x0a,0x10,0x11,0x12,
    0x13,0x00,0x00,0x00,0x00,0x04,0x00,0x02,
    0x02,0x02,0x02,0x00,0x00,0x00,0x04,0x04,
    0x04,0x04,0x00,0x00,0x01,0x06,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const char PROGMEM buf3f2[] = {
    0xf2,
    0xff,0xff,0x00,
    0x01,0x02,0x03,0x04,0x05,0x06, //device bdaddr
    0x00,0x03,0x50,0x81,0xd8,0x01,0x8a,0x00,
    0x00,0x01,0x64,0x19,0x01,0x00,0x64,0x00,
    0x01,0x90,0x00,0x19,0xfe,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,
};

char buf3f5[] = {
    0x01,0x00,
    0xaa,0xaa,0xaa,0xaa,0xaa,0xaa, //dummy PS3 bdaddr
    0x23,0x1e,0x00,0x03,0x50,0x81,0xd8,0x01,
    0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,
    0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

char buf3ef[] = {
    0x00,0xef,0x03,0x00,0x04,0x03,0x01,0xb0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x02,0x05,0x01,0x92,0x02,0x02,0x01,
    0x91,0x02,0x05,0x01,0x91,0x02,0x04,0x00,
    0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

char buf3f8[] = {
    0x00,0x01,0x00,0x00,0x07,0x03,0x01,0xb0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x02,0x6b,0x02,0x68,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const char PROGMEM buf3f7[] = {
    0x01,0x00,0x08,0x03,0xd2,0x01,0xee,0xff,
    0x10,0x02,0x00,0x03,0x50,0x81,0xd8,0x01,
    0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,
    0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

/** Event handler for the USB_UnhandledControlRequest event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the HID commands, which are
 *  all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
void EVENT_USB_Device_UnhandledControlRequest(void)
{
  /* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
        if(USB_ControlRequest.wValue == 0x0301)
        {
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_PStream_LE(buf301, USB_ControlRequest.wLength);
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03f2)
        {
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_PStream_LE(buf3f2, USB_ControlRequest.wLength);
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03f5)
        {
          Endpoint_ClearSETUP();
          if(reply == 0)
          {
            /*
             * First request, tell that the bdaddr is not the one of the PS3.
             */
            Endpoint_Write_Control_Stream_LE(buf3f5, USB_ControlRequest.wLength);
            reply = 1;
          }
          else
          {
            /*
             * Next requests, tell that the bdaddr is the one of the PS3.
             */
            memcpy(buf3f5+2, VolatileMasterBdaddr, 6);
            Endpoint_Write_Control_Stream_LE(buf3f5, USB_ControlRequest.wLength);
          }
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03ef)
        {
          buf3ef[7] = last_set_data[6];
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(buf3ef, USB_ControlRequest.wLength);
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03f8)
        {
          buf3f8[7] = last_set_data[6];
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(buf3f8, USB_ControlRequest.wLength);
          Endpoint_ClearOUT();
        }
        else if(USB_ControlRequest.wValue == 0x03f7)
        {
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_PStream_LE(buf3f7, USB_ControlRequest.wLength);
          Endpoint_ClearOUT();
        }
			}
		
			break;
		case REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				uint8_t i;
				Endpoint_ClearSETUP();
				
				/* Wait until the LED report has been sent by the host */
				while (!(Endpoint_IsOUTReceived()))
				{
					if (USB_DeviceState == DEVICE_STATE_Unattached)
					  return;
				}

				for(i=0; i<USB_ControlRequest.wLength; i++)
				{
					last_set_data[i] = Endpoint_Read_Byte();
				}

				/* Clear the endpoint data */
				Endpoint_ClearOUT();

				Endpoint_ClearStatusStage();

				if(USB_ControlRequest.wValue == 0x03f5)
				{
					memcpy(VolatileMasterBdaddr, last_set_data+2, 6);
				}
			}
			
			break;
		case REQ_GetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				
				/* Write the current protocol flag to the host */
				Endpoint_Write_Byte(UsingReportProtocol);
				
				/* Send the flag to the host */
				Endpoint_ClearIN();

				Endpoint_ClearStatusStage();
			}
			
			break;
		case REQ_SetProtocol:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();

				/* Set or clear the flag depending on what the host indicates that the current Protocol should be */
				UsingReportProtocol = (USB_ControlRequest.wValue != 0);

				Endpoint_ClearStatusStage();
			}
			
			break;
		case REQ_SetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				
				/* Get idle period in MSB, IdleCount must be multiplied by 4 to get number of milliseconds */
				IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
				
				Endpoint_ClearStatusStage();
			}
			
			break;
		case REQ_GetIdle:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{		
				Endpoint_ClearSETUP();
				
				/* Write the current idle duration to the host, must be divided by 4 before sent to host */
				Endpoint_Write_Byte(IdleCount >> 2);
				
				/* Send the flag to the host */
				Endpoint_ClearIN();

				Endpoint_ClearStatusStage();
			}

			break;
	}
}

/** Event handler for the USB device Start Of Frame event. */
void EVENT_USB_Device_StartOfFrame(void)
{
	/* One millisecond has elapsed, decrement the idle time remaining counter if it has not already elapsed */
	if (IdleMSRemaining)
	  IdleMSRemaining--;
}

/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param[in] LEDReport  LED status report from the host
 */
void ProcessLEDReport(uint8_t LEDReport)
{
	
}

static int led = 0;
static unsigned int j = 0;

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void SendNextReport(void)
{
	static uint8_t prevReport[sizeof(report)];
	bool SendReport = true;
	
	/* Check to see if the report data has changed - if so a report MUST be sent */
	SendReport = (memcmp(prevReport, report, sizeof(report)) != 0);
	
	/* Check if the idle period is set and has elapsed */
	if ((IdleCount != HID_IDLE_CHANGESONLY) && (!(IdleMSRemaining)))
	{
		/* Reset the idle time remaining counter */
		IdleMSRemaining = IdleCount;
		
		/* Idle period is set and has elapsed, must send a report to the host */
		SendReport = true;
	}
	
	/* Select the Keyboard Report Endpoint */
	Endpoint_SelectEndpoint(SIXAXIS_IN_EPNUM);

	/* Check if Keyboard Endpoint Ready for Read/Write and if we should send a new report */
	if (Endpoint_IsReadWriteAllowed() && SendReport)
	{
		/* Save the current report data for later comparison to check for changes */
		memcpy(prevReport, report, sizeof(report));
	
		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(report, sizeof(report));
		
		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();

		++j;
    if(!(j%250))
    {
      if(led)
      {
        LED_OFF;
        led = 0;
      }
      else
      {
        LED_ON;
        led = 1;
      }
    }
	}
}

/** Reads the next LED status report from the host from the LED data endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
	/* Select the Keyboard LED Report Endpoint */
	Endpoint_SelectEndpoint(SIXAXIS_OUT_EPNUM);

	/* Check if Keyboard LED Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Read in the LED report from the host */
			uint8_t LEDReport = Endpoint_Read_Byte();

			/* Process the read LED report from the host */
			ProcessLEDReport(LEDReport);
		}

		/* Handshake the OUT Endpoint - clear endpoint and ready for next report */
		Endpoint_ClearOUT();
	}
}

/** Function to manage HID report generation and transmission to the host, when in report mode. */
void HID_Task(void)
{
	/* Device must be connected and configured for the task to run */
	if (USB_DeviceState != DEVICE_STATE_Configured)
	  return;
	  
	/* Send the next keypress report to the host */
	SendNextReport();
		
	/* Process the LED report sent from the host */
	ReceiveNextReport();
}
