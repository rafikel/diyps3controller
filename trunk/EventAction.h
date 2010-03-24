/*
 ********************************************************************************
 * Copyright (c) 2009 Richard Burke
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
 * $Rev: 191 $
 * $Author: rburke $
 * $LastChangedDate: 2009-11-16 00:48:21 -0500 (Mon, 16 Nov 2009) $
 ********************************************************************************
 */
#ifndef __EVENTACTION_H__
#define __EVENTACTION_H__

typedef enum { Null, Keyboard, Mouse, Usb, XBox, Arcade, Joystick, Script } Device_t;

/*
 ****************************************************************************************************
 * Events
 *
 *   ,----------------------------------------------------  Device : 0 - 15
 *   |             ,-------------------------------------- Trigger : 0 - 15
 *   v             v             v------------------------   Value : 0 - 255
 *  ------------- ------------- -------------------------
 * | D3 D2 D1 D0 | T3 T2 T1 T0 | V7 V6 V5 V4 V3 V2 V1 V0 |
 *  ------------- ------------- -------------------------
 * |            Key            |         Value           |
 *  --------------------------- -------------------------
 *
 ****************************************************************************************************
 */
#define EVA_NewEvent(k,v)       ((uint16_t)((k)|(v)))
#define EVA_NewEventKey(d,t)    ((uint16_t)(((d)<<12)|((t)<<8)))
#define EVA_NewEventValue(v)    ((uint16_t)(v))

#define EVA_EventKey(e)         ((uint8_t)(((e)>>8)&0xff))
#define EVA_EventDevice(e)      ((uint8_t)(((e)>>12)&0x0f))
#define EVA_EventTrigger(e)     ((uint8_t)(((e)>>8)&0x0f))
#define EVA_EventValue(e)       ((uint8_t)((e)&0xff))

#define EVA_NullEvent           ((uint16_t) 0)

/*
 ****************************************************************************************************
 * Actions
 *
 *   ,---------------------------------------------------- Device ( 0 - 15 )
 *   |             ,-------------------------------------- Target ( 0 - 15 )
 *   v             v             v------------------------ Value (0 - 255)
 *  ------------- ------------- -------------------------
 * | D0 D1 D2 D3 | T0 T1 T2 T3 | V7 V6 V5 V4 V3 V2 V1 V0 |
 *  ------------- ------------- -------------------------
 * |            Key            |         Value           |
 *  --------------------------- -------------------------
 *
 ****************************************************************************************************
 */
#define EVA_NewAction(k,v)      ((uint16_t)((k)|(v)))
#define EVA_NewActionKey(d,t)   ((uint16_t)(((d)<<12)|((t)<<8)))
#define EVA_NewActionValue(v)   ((uint16_t)(v))

#define EVA_ActionKey(a)        ((uint8_t)(((a)>>8)&0xff))
#define EVA_ActionDevice(a)     ((uint8_t)(((a)>>12)&0x0f))
#define EVA_ActionTarget(a)     ((uint8_t)(((a)>>8)&0x0f))
#define EVA_ActionValue(a)      ((uint8_t)((a)&0xff))

#define EVA_NullAction          ((uint16_t) 0)

/*
 ********************************************************************************
 * GlobalFunctions
 ********************************************************************************
 */
extern void EVA_PerformAction( uint16_t event );
extern void EVA_InvokeAction( uint16_t event, uint16_t action );
extern void EVA_InvokeEventAction( uint16_t event );

#endif
