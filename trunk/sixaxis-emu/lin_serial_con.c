/*
 * lin_serial_con.c
 *
 *  Created on: 9 juin 2011
 *      Author: matlo

 *  License: GPLv3
 */

#include "lin_serial_con.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

/*
 * The serial connection.
 */
int serial;

/*
 * The baud rate in bps.
 */
#define BAUDRATE B500000

/*
 * Connect to a serial port.
 */
int lin_serial_connect(char* portname)
{
  struct termios options;

  printf("connecting to %s\n", portname);

  if ((serial = open(portname, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
  {
    printf("can't connect to %s\n", portname);
    exit(-1);
  }

  tcgetattr(serial, &options);
  cfsetispeed(&options, BAUDRATE);
  cfsetospeed(&options, BAUDRATE);
  options.c_cflag |= (CLOCAL | CREAD);
  tcsetattr(serial, TCSANOW, &options);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  printf("connected\n");

  return 0;
}

/*
 * Send a usb report to the serial port.
 */
int lin_serial_send(s_report_data* pdata)
{
  return write(serial, (uint8_t*)pdata, sizeof(s_report_data));
}

void lin_serial_close()
{
  close(serial);
}
