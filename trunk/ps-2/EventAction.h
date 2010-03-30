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
