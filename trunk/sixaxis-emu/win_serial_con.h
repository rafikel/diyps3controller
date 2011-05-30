/*
 * win_serial.h
 *
 *  Created on: 28 mai 2011
 *      Author: Matlo
 *
 *  License: GPLv3
 */

#ifndef WIN_SERIAL_CON_H_
#define WIN_SERIAL_CON_H_

#include "sixaxis.h"

int serial_connect(char*);
int serial_send(struct sixaxis_state*);

#endif /* WIN_SERIAL_H_ */
