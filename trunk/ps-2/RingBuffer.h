/*
 ********************************************************************************
 * Ringbuffer.h
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
 * $Rev: 220 $
 * $Author: rburke $
 * $LastChangedDate: 2009-11-26 12:19:45 -0500 (Thu, 26 Nov 2009) $
 *
 ********************************************************************************
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
