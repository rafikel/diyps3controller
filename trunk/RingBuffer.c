/*
 ********************************************************************************
 * RingBuffer.c
 *
 * Copyright (C) 2009 Richard Burke
 * Copyright (c) 2010 Mathieu Laurendeau
 * All rights reserved.
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
 * $Rev: 127 $
 * $Author: rburke $
 * $LastChangedDate: 2009-08-24 00:06:03 -0400 (Mon, 24 Aug 2009) $
 *
 ********************************************************************************
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
