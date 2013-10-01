/*
  Copyright 2013  Mathieu Laurendeau (mat.lau [at] laposte [dot] net)

  Redistributed under the GPLv3 licence.

  Based on code by
    Denver Gingerich (denver [at] ossguy [dot] com)
    Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Original licence:

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
 *  Main source file for the sixaxis controller. This file contains the main tasks and
 *  is responsible for the initial application hardware configuration.
 */
 
#include "sixaxis_emu.h"
#include <LUFA/Drivers/Peripheral/Serial.h>

#define USART_BAUDRATE 500000

#define LED_PIN 6

#define LED_ON (PORTD |= (1<<LED_PIN))
#define LED_OFF (PORTD &= ~(1<<LED_PIN))

/*
 * The master bdaddr.
 */
static uint8_t masterBdaddr[6];

/*
 * A byte to save.
 */
static uint8_t byte_6_ef;

/*
 * Indicates if the master bdaddr was already requested or not.
 */
static unsigned char reply = 0;

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

static uint8_t* pdata = report;
static unsigned char i = 0;

static unsigned char sendReport = 0;

static inline int16_t Serial_BlockingReceiveByte(void)
{
  while(!Serial_IsCharReceived());
  return UDR1;
}

/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
  SetupHardware();

  GlobalInterruptEnable();

  for (;;)
  {
    HID_Task();
    USB_USBTask();
  }
}

ISR(USART1_RX_vect)
{
  pdata[i++] = UDR1;
  while(i < sizeof(report))
  {
    pdata[i++] = Serial_BlockingReceiveByte();
  }
  i = 0;
  sendReport = 1;
}

void serial_init(void)
{
  Serial_Init(USART_BAUDRATE, false);

  UCSR1B |= (1 << RXCIE1); // Enable the USART Receive Complete interrupt (USART_RXC)
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
  DDRD |= (1<<LED_PIN);

  USB_Init();
}

/** Event handler for the USB_Connect event. This indicates that the device is enumerating via the status LEDs and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Device_Connect(void)
{
	/* Indicate USB enumerating */
	LEDs_SetAllLEDs(LEDMASK_USB_ENUMERATING);
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
 *  of the USB device after enumeration, and configures the device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void)
{	
	/* Indicate USB connected and ready */
	LEDs_SetAllLEDs(LEDMASK_USB_READY);

	/* Setup IN Report Endpoint */
	if (!(Endpoint_ConfigureEndpoint(SIXAXIS_IN_EPNUM, EP_TYPE_INTERRUPT, SIXAXIS_EPSIZE, 1)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	/* Setup OUT LED Report Endpoint */
	if (!(Endpoint_ConfigureEndpoint(SIXAXIS_OUT_EPNUM, EP_TYPE_INTERRUPT, SIXAXIS_EPSIZE, 1)))
	{
		LEDs_SetAllLEDs(LEDMASK_USB_ERROR);
	}
	
	//USB_Device_EnableSOFEvents();
}

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

const char PROGMEM buf3f5[] = {
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

const char PROGMEM buf3ef[] = {
    0x00,0xef,0x03,0x00,0x04,0x03,0x01,0xb0,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x02,0x05,0x01,0x92,0x02,0x02,0x01,
    0x91,0x02,0x05,0x01,0x91,0x02,0x04,0x00,
    0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

const char PROGMEM buf3f8[] = {
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

/** Event handler for the USB_ControlRequest event. This is used to catch and process control requests sent to
 *  the device from the USB host before passing along unhandled control requests to the library for processing
 *  internally.
 */
void EVENT_USB_Device_ControlRequest(void)
{
  static char buffer[64];

  /* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
        if(USB_ControlRequest.wValue == 0x0301)
        {
          memcpy_P(buffer, buf301, sizeof(buffer));
        }
        else if(USB_ControlRequest.wValue == 0x03f2)
        {
          memcpy_P(buffer, buf3f2, sizeof(buffer));
        }
        else if(USB_ControlRequest.wValue == 0x03f5)
        {
          memcpy_P(buffer, buf3f5, sizeof(buffer));
          if(reply == 0)
          {
            /*
             * First request, tell that the bdaddr is not the one of the PS3.
             */
            reply = 1;
          }
          else
          {
            /*
             * Next requests, tell that the bdaddr is the one of the PS3.
             */
            memcpy(buffer+2, masterBdaddr, 6);
          }
        }
        else if(USB_ControlRequest.wValue == 0x03ef)
        {
          memcpy_P(buffer, buf3ef, sizeof(buffer));
          buffer[7] = byte_6_ef;
        }
        else if(USB_ControlRequest.wValue == 0x03f8)
        {
          memcpy_P(buffer, buf3f8, sizeof(buffer));
          buffer[7] = byte_6_ef;//necessary??
        }
        else if(USB_ControlRequest.wValue == 0x03f7)
        {
          memcpy_P(buffer, buf3f7, sizeof(buffer));
        }
        else
        {
          break;
        }
        Endpoint_ClearSETUP();
        Endpoint_Write_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
        Endpoint_ClearOUT();
			}
		
			break;
		case REQ_SetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				Endpoint_ClearSETUP();
				
				Endpoint_Read_Control_Stream_LE(buffer, USB_ControlRequest.wLength);
				Endpoint_ClearIN();

				if(USB_ControlRequest.wValue == 0x03f5)
				{
					memcpy(masterBdaddr, buffer+2, 6);
				}
				else if(USB_ControlRequest.wValue == 0x03ef)
        {
          byte_6_ef = buffer[6];
        }
			}
			
			break;

	}
}

/** Event handler for the USB device Start Of Frame event. */
/*void EVENT_USB_Device_StartOfFrame(void)
{
	// One millisecond has elapsed.
}*/


/** Sends the next HID report to the host, via the IN endpoint. */
void SendNextReport(void)
{
	/* Select the IN Report Endpoint */
	Endpoint_SelectEndpoint(SIXAXIS_IN_EPNUM);

  if (sendReport)
  {
    /* Wait until the host is ready to accept another packet */
    while (!Endpoint_IsINReady()) {}

		/* Write IN Report Data */
		Endpoint_Write_Stream_LE(report, sizeof(report), NULL);
		
		sendReport = 0;

		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
	}
}

/** Reads the next OUT report from the host from the OUT endpoint, if one has been sent. */
void ReceiveNextReport(void)
{
	/* Select the OUT Report Endpoint */
	Endpoint_SelectEndpoint(SIXAXIS_OUT_EPNUM);

	/* Check if OUT Endpoint contains a packet */
	if (Endpoint_IsOUTReceived())
	{
		/* Check to see if the packet contains data */
		if (Endpoint_IsReadWriteAllowed())
		{
			/* Discard data */
			Endpoint_Discard_8();
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