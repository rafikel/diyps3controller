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
#include <avr/eeprom.h>

uint8_t EEMEM DeviceBdaddr[6] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
uint16_t EEMEM VendorId = 0x03EB;
uint16_t EEMEM ProductId = 0x2042;

/*
 * External declaration, to set the product id and vendor id.
 */
extern USB_Descriptor_Device_t DeviceDescriptor;

/*
 * The device bdaddr, read from eeprom at startup.
 */
uint8_t VolatileDeviceBdaddr[6];

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
static USB_SixaxisReport_Data_t RefReportData = {
	.buffer = {
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
	}
};

/*
 * Reads the vendor id, the product id, and the bdaddr from the eeprom.
 */
void readEepromIdentifiers(void)
{
	DeviceDescriptor.VendorID = eeprom_read_word(&VendorId);
	DeviceDescriptor.ProductID = eeprom_read_word(&ProductId);
	if(DeviceDescriptor.VendorID == 0xFFFF) DeviceDescriptor.VendorID = 0x03EB;
	if(DeviceDescriptor.ProductID == 0xFFFF) DeviceDescriptor.ProductID = 0x2042;
	eeprom_read_block((void*)&VolatileDeviceBdaddr, (const void*)&DeviceBdaddr, 6);
}


/** Main program entry point. This routine configures the hardware required by the application, then
 *  enters a loop to run the application tasks in sequence.
 */
int main(void)
{
	int i;

	readEepromIdentifiers();

	SetupHardware();
	
	for (i=0;i<2000;++i)
	{
		_delay_ms(1);
		HID_Task();
		USB_USBTask();
	}

	/*
	 * Send PS3 button press after 2s
	 */

	RefReportData.buffer[4] = 0x01;


	for (i=0;i<1000;++i)
	{
		_delay_ms(1);
		HID_Task();
		USB_USBTask();
	}

	/*
	 * Send PS3 button release after 0.5s
	 */

	RefReportData.buffer[4] = 0x00;

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

	/* Hardware Initialization */
	LEDs_Init();
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

/** Event handler for the USB_UnhandledControlRequest event. This is used to catch standard and class specific
 *  control requests that are not handled internally by the USB library (including the HID commands, which are
 *  all issued via the control endpoint), so that they can be handled appropriately for the application.
 */
void EVENT_USB_Device_UnhandledControlRequest(void)
{
	/*
	 * For debug purposes!
	log[nb_requests++] = USB_ControlRequest;
	*/
	/* Handle HID Class specific requests */
	switch (USB_ControlRequest.bRequest)
	{
		case REQ_GetReport:
			if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
			{
				if(USB_ControlRequest.wValue == 0xDEAD)
				{
					char buf[64];
					memcpy(buf, &(DeviceDescriptor.VendorID), 2);
					memcpy(buf+2, &(DeviceDescriptor.ProductID), 2);
					memcpy(buf+4, VolatileDeviceBdaddr, 6);
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x03f7) == 0x03f7)
				{
					char buf[] = {0x01,0x00,0x08,0x03,0xd2,0x01,0xee,0xff,0x10,0x02,0x00,0x03,0x50,0x81,0xd8,0x01,0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x03f5) == 0x03f5)
				{
					Endpoint_ClearSETUP();
					if(reply == 0)
					{
						/*
						 * First request, tell that the bdaddr is not the one of the PS3.
						 */

						char buf[] = {0x01,0x00,
								0xaa,0xaa,0xaa,0xaa,0xaa,0xaa, //dummy PS3 bdaddr
								0x23,0x1e,0x00,0x03,0x50,0x81,0xd8,0x01,0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};

						Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
						reply = 1;
					}
					else
					{
						/*
						 * Next requests, tell that the bdaddr is the one of the PS3.
						 */

						char buf[] = {0x01,0x00,
								0x00,0x00,0x00,0x00,0x00,0x00, //PS3 bdaddr, set by the next memcpy
								0x23,0x1e,0x00,0x03,0x50,0x81,0xd8,0x01,0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
						memcpy(buf+2, VolatileMasterBdaddr, 6);
						Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					}
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x03f2) == 0x03f2)
				{
					char buf[] = {0xf2,0xff,0xff,0x00,
							0x00,0x00,0x00,0x00,0x00,0x00, //device bdaddr, set by the next memcpy
							0x00,0x03,0x50,0x81,0xd8,0x01,0x8a,0x00,0x00,0x01,0x64,0x19,0x01,0x00,0x64,0x00,0x01,0x90,0x00,0x19,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					memcpy(buf+4, VolatileDeviceBdaddr, 6);
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x03ef) == 0x03ef)
				{
				    char buf[] = {0x00,0xef,0x03,0x00,0x04,0x03,0x01,0xb0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x05,0x01,0x92,0x02,0x02,0x01,0x91,0x02,0x05,0x01,0x91,0x02,0x04,0x00,0x76,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					buf[7] = last_set_data[6];
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x03ee) == 0x03ee)
				{
					char buf[] = {0x00,0xee,0x02,0x00,0x04,0xee,0x10,0x00,0x00,0x00,0x00,0x02,0x03,0x08,0x01,0xd2,0x00,0x00,0x02,0x02,0x02,0x00,0x03,0x00,0x00,0x02,0x00,0x00,0x02,0x62,0x01,0x02,0x01,0x5e,0x00,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x0302) == 0x0302)
				{
					char buf[] = {0x00,0x02,0x01,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x0301) == 0x0301)
				{
					char buf[] = {0x00,0x01,0x03,0x00,0x04,0x0c,0x01,0x02,0x18,0x18,0x18,0x18,0x09,0x0a,0x10,0x11,0x12,0x13,0x00,0x00,0x00,0x00,0x04,0x00,0x02,0x02,0x02,0x02,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x00,0x00,0x01,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x0201) == 0x0201)
				{
					char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0xff,0x27,0x10,0x32,0x32,0xff,0x27,0x10,0x32,0x32,0xff,0x27,0x10,0x32,0x32,0xff,0x27,0x10,0x32,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else if((USB_ControlRequest.wValue & 0x0101) == 0x0101)
				{
					char buf[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x80,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xee,0x10,0x00,0x00,0x00,0x00,0x02,0x54,0x77,0x01,0x80,0x01,0xff,0x01,0xff,0x01,0xff,0x01,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
					Endpoint_ClearSETUP();
					Endpoint_Write_Control_Stream_LE(buf, USB_ControlRequest.wLength);
					Endpoint_ClearOUT();
				}
				else
				{
					USB_SixaxisReport_Data_t SixaxisReportData;

					Endpoint_ClearSETUP();

					/* Create the next keyboard report for transmission to the host */
					CreateSixaxisReport(&SixaxisReportData);
	
					/* Write the report data to the control endpoint */
					Endpoint_Write_Control_Stream_LE(&SixaxisReportData, sizeof(SixaxisReportData));

					/* Finalize the stream transfer to send the last packet or clear the host abort */
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

				if(USB_ControlRequest.wValue == 0xDEAD)
				{
					eeprom_write_block(last_set_data, &VendorId, 2);
					eeprom_write_block(last_set_data+2, &ProductId, 2);
					eeprom_write_block(last_set_data+4, &DeviceBdaddr, 6);
				}

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



/** Fills the given HID report data structure with the next HID report to send to the host.
 *
 *  \param[out] ReportData  Pointer to a HID report data structure to be filled
 */
void CreateSixaxisReport(USB_SixaxisReport_Data_t* ReportData)
{
	memcpy(ReportData, &RefReportData, sizeof(USB_SixaxisReport_Data_t));
}

/** Processes a received LED report, and updates the board LEDs states to match.
 *
 *  \param[in] LEDReport  LED status report from the host
 */
void ProcessLEDReport(uint8_t LEDReport)
{
	
}

/** Sends the next HID report to the host, via the keyboard data endpoint. */
void SendNextReport(void)
{
	static USB_SixaxisReport_Data_t PrevSixaxisReportData;
	USB_SixaxisReport_Data_t        SixaxisReportData;
	bool                             SendReport = true;
	
	/* Create the next keyboard report for transmission to the host */
	CreateSixaxisReport(&SixaxisReportData);
	
	/* Check to see if the report data has changed - if so a report MUST be sent */
	SendReport = (memcmp(&PrevSixaxisReportData, &SixaxisReportData, sizeof(USB_SixaxisReport_Data_t)) != 0);
	
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
		PrevSixaxisReportData = SixaxisReportData;
	
		/* Write Keyboard Report Data */
		Endpoint_Write_Stream_LE(&SixaxisReportData, sizeof(SixaxisReportData));
		
		/* Finalize the stream transfer to send the last packet */
		Endpoint_ClearIN();
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
