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

#include <util/atomic.h>
#include "RingBuffer.h"

/*
 ********************************************************************************
 * RING_Initialize
 ********************************************************************************
 */
void RING_Initialize( ringbuff_t* buffer, uint8_t *data, uint8_t size )
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		buffer->data = data;
		buffer->size = size;
		buffer->in = 0;
		buffer->out = 0;
		buffer->elements = 0;
		for ( uint8_t i = 0; i < buffer->size; i++ )
			buffer->data[i] = 0;
	}
}

/*
 ********************************************************************************
 * RING_AddElement
 ********************************************************************************
 */
uint8_t RING_AddElement( ringbuff_t* buffer, uint8_t data )
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if( buffer->elements < buffer->size )
		{
			buffer->data[buffer->in++] = data;
			if( buffer->in == buffer->size )
				buffer->in = 0;
			buffer->elements++;
		}
	}

	return data;
}

/*
 ********************************************************************************
 * RING_GetElement
 ********************************************************************************
 */
uint8_t RING_GetElement( ringbuff_t* buffer )
{
	uint8_t data = 0;
	
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if( buffer->elements > 0 )
		{
			data = buffer->data[buffer->out++];
			if( buffer->out == buffer->size )
				buffer->out = 0;
			buffer->elements--;
		}
	}

	return data;
}

/*
 ********************************************************************************
 * RING_HasElement
 ********************************************************************************
 */
uint8_t RING_HasElement( ringbuff_t* buffer )
{
	return buffer->elements > 0;
}
