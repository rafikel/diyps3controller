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

#ifndef _RINGBUFF_H_
#define _RINGBUFF_H_

#define	RING_DEFAULT_SIZE	64
#define RING_RECIEVE_SIZE	64 + 16
#define RING_TRANSMIT_SIZE	128

typedef struct _ringbuff_t
{
	uint8_t *data;			// Physical memory address where the buffer is stored
	uint8_t size;			// Allocated size of the buffer
	uint8_t in;				// Index into the buffer
	uint8_t	out;			// Index from the buffer
	uint8_t elements;		// Number of bytes currently in the buffer
} ringbuff_t;

/* Function Prototypes: */
extern void RING_Initialize( ringbuff_t* buffer, uint8_t *data, uint8_t size );
extern uint8_t RING_AddElement( ringbuff_t* buffer, uint8_t data );
extern uint8_t RING_GetElement( ringbuff_t* buffer );
extern uint8_t RING_HasElement( ringbuff_t* buffer );

#endif
