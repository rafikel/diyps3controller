/*
 * win_serial.c
 *
 *  Created on: 28 mai 2011
 *      Author: Matlo
 *  
 *  License: GPLv3
 */

#include "win_serial_con.h"
#include <windows.h>
#include <sys/time.h>
#include <stdio.h>
#include "config.h"

/*
 * The usb report structure to send over the serial connection.
 */
typedef struct
{
  int16_t X;
  int16_t Y;
  int16_t Z;
  int16_t Rz;
  uint16_t Hat;
  uint16_t Bt;
} s_report_data;

/*
 * The serial connection.
 */
static HANDLE hSerial;

/*
 * The baud rate in bps.
 */
static int baudrate = 500000;

static int debug;

/*
 * Imported from emuclient.c
 */
extern int max_axis_value;
extern int mean_axis_value;

extern s_controller controller[MAX_CONTROLLERS];
extern struct sixaxis_state state[MAX_CONTROLLERS];

/*
 * Connect to a serial port.
 */
int serial_connect(char* portname)
{
  int ret = 0;
  DWORD accessdirection = /*GENERIC_READ |*/GENERIC_WRITE;
  hSerial = CreateFile(portname, accessdirection, 0, 0, OPEN_EXISTING, 0, 0);
  if (hSerial == INVALID_HANDLE_VALUE)
  {
    ret = -1;
  }
  DCB dcbSerialParams =
  { 0 };
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(hSerial, &dcbSerialParams))
  {
    ret = -1;
  }
  dcbSerialParams.BaudRate = baudrate;
  dcbSerialParams.ByteSize = 8;
  dcbSerialParams.StopBits = ONESTOPBIT;
  dcbSerialParams.Parity = NOPARITY;
  if (!SetCommState(hSerial, &dcbSerialParams))
  {
    ret = -1;
  }
  COMMTIMEOUTS timeouts =
  { 0 };
  timeouts.ReadIntervalTimeout = 50;
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;
  if (!SetCommTimeouts(hSerial, &timeouts))
  {
    ret = -1;
  }
  return ret;
}

/*
 * Send a usb report to the serial port.
 */
int serial_send()
{
  s_report_data data =
  { };
  int i;
  struct timeval tv;
  uint8_t* pdata = (uint8_t*) &data;
  DWORD dwBytesWrite = 0;

  /*
   * Make sure the value is not out of range.
   */
  data.X = clamp(0, state[0].user.axis[0][0] + mean_axis_value, max_axis_value);
  data.Y = clamp(0, state[0].user.axis[0][1] + mean_axis_value, max_axis_value);
  data.Z = clamp(0, state[0].user.axis[1][0] + mean_axis_value, max_axis_value);
  data.Rz = clamp(0, state[0].user.axis[1][1] + mean_axis_value, max_axis_value);
  if (state[0].user.button[sb_square].pressed)
  {
    data.Bt |= 0x0001;
  }
  if (state[0].user.button[sb_cross].pressed)
  {
    data.Bt |= 0x0002;
  }
  if (state[0].user.button[sb_circle].pressed)
  {
    data.Bt |= 0x0004;
  }
  if (state[0].user.button[sb_triangle].pressed)
  {
    data.Bt |= 0x0008;
  }

  if (state[0].user.button[sb_select].pressed)
  {
    data.Bt |= 0x0100;
  }
  if (state[0].user.button[sb_start].pressed)
  {
    data.Bt |= 0x0200;
  }
  if (state[0].user.button[sb_l3].pressed)
  {
    data.Bt |= 0x0400;
  }
  if (state[0].user.button[sb_r3].pressed)
  {
    data.Bt |= 0x0800;
  }

  if (state[0].user.button[sb_l1].pressed)
  {
    data.Bt |= 0x0010;
  }
  if (state[0].user.button[sb_r1].pressed)
  {
    data.Bt |= 0x0020;
  }
  if (state[0].user.button[sb_l2].pressed)
  {
    data.Bt |= 0x0040;
  }
  if (state[0].user.button[sb_r2].pressed)
  {
    data.Bt |= 0x0080;
  }

  if (state[0].user.button[sb_right].pressed)
  {
    if (state[0].user.button[sb_down].pressed)
    {
      data.Hat = 0x0003;
    }
    else if (state[0].user.button[sb_up].pressed)
    {
      data.Hat = 0x0001;
    }
    else
    {
      data.Hat = 0x0002;
    }
  }
  else if (state[0].user.button[sb_left].pressed)
  {
    if (state[0].user.button[sb_down].pressed)
    {
      data.Hat = 0x0005;
    }
    else if (state[0].user.button[sb_up].pressed)
    {
      data.Hat = 0x0007;
    }
    else
    {
      data.Hat = 0x0006;
    }
  }
  else if (state[0].user.button[sb_down].pressed)
  {
    data.Hat = 0x0004;
  }
  else if (state[0].user.button[sb_up].pressed)
  {
    data.Hat = 0x0000;
  }
  else
  {
    data.Hat = 0x0008;
  }

  /* Dump contents */
  if (debug)
  {
    gettimeofday(&tv, NULL);
    printf("%ld.%06ld ", tv.tv_sec, tv.tv_usec);
    for (i = 0; i < sizeof(s_report_data); i++)
    {
      printf(" %02x", pdata[i]);
    }
    printf("\n");
  }

  return WriteFile(hSerial, pdata, sizeof(s_report_data), &dwBytesWrite, NULL);
}
