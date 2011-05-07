/*
 * config.h
 *
 *  Created on: 30 oct. 2010
 *      Author: matlo
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <SDL/SDL.h>
#include "sixaxis.h"

#ifndef WIN32
#define CONFIG_DIR ".emuclient/config"
#else
#define CONFIG_DIR "config"
#endif

#define MAX_CONTROLLERS 7
#define MAX_CONFIGURATIONS 8
#define MAX_DEVICES 256
#define MAX_CONTROLS 256
#define POSTPONE_COUNT 3

#define X_NODE_ROOT "root"
#define X_NODE_CONTROLLER "controller"
#define X_NODE_CONFIGURATION "configuration"
#define X_NODE_TRIGGER "trigger"
#define X_NODE_INTENSITY_LIST "intensity_list"
#define X_NODE_INTENSITY "intensity"
#define X_NODE_BUTTON_MAP "button_map"
#define X_NODE_AXIS_MAP "axis_map"
#define X_NODE_DEVICE "device"
#define X_NODE_EVENT "event"
#define X_NODE_AXIS "axis"
#define X_NODE_BUTTON "button"
#define X_NODE_UP "up"
#define X_NODE_DOWN "down"

#define X_ATTR_ID "id"
#define X_ATTR_TYPE "type"
#define X_ATTR_NAME "name"
#define X_ATTR_BUTTON_ID "button_id"
#define X_ATTR_THRESHOLD "threshold"
#define X_ATTR_DEADZONE "dead_zone"
#define X_ATTR_MULTIPLIER "multiplier"
#define X_ATTR_EXPONENT "exponent"
#define X_ATTR_SHAPE "shape"
#define X_ATTR_SWITCH_BACK "switch_back"
#define X_ATTR_STEPS "steps"
#define X_ATTR_CONTROL "control"

#define X_ATTR_VALUE_KEYBOARD "keyboard"
#define X_ATTR_VALUE_MOUSE "mouse"
#define X_ATTR_VALUE_JOYSTICK "joystick"
#define X_ATTR_VALUE_BUTTON "button"
#define X_ATTR_VALUE_AXIS "axis"
#define X_ATTR_VALUE_AXIS_DOWN "axis_down"
#define X_ATTR_VALUE_AXIS_UP "axis_up"
#define X_ATTR_VALUE_CIRCLE "Circle"
#define X_ATTR_VALUE_RECTANGLE "Rectangle"
#define X_ATTR_VALUE_YES "yes"
#define X_ATTR_VALUE_NO "no"

typedef enum
{
  E_DEVICE_TYPE_UNKNOWN,
  E_DEVICE_TYPE_KEYBOARD,
  E_DEVICE_TYPE_MOUSE,
  E_DEVICE_TYPE_JOYSTICK
}e_device_type;

typedef enum
{
  E_EVENT_TYPE_UNKNOWN,
  E_EVENT_TYPE_BUTTON,
  E_EVENT_TYPE_AXIS,
  E_EVENT_TYPE_AXIS_DOWN,
  E_EVENT_TYPE_AXIS_UP
}e_event_type;

typedef enum
{
    E_SHAPE_CIRCLE,
    E_SHAPE_RECTANGLE
}e_shape;

typedef enum
{
  NONE,
  MC,
  CC,
  MX,
  MY,
  DZX,
  DZY,
  DZS,
  RD,
  EX,
  EY,
  TEST
}e_current_cal;

typedef struct
{
  int change;
  int changed;
  double merge_x;
  double merge_y;
  int postpone_wheel_up;
  int postpone_wheel_down;
  int postpone_button_x1;
  int postpone_button_x2;
}s_mouse_control;

typedef struct
{
  double* mx;
  double* my;
  double* ex;
  double* ey;
  double rd;
  int* dzx;
  int* dzy;
  e_shape* dzs;
}s_mouse_cal;

typedef struct
{
  int change;
  int send_command;
  int ts_axis[TS_MAX][TS_AXIS_MAX][2];
} s_controller;

typedef struct
{
  int nb_mappers;

  int button;
  int axis;
  int threshold;
  double multiplier;
  double exponent;
  e_shape shape;
  int dead_zone;

  int controller_button;
  int controller_button_axis;
  int controller_thumbstick;
  int controller_thumbstick_axis;
  int controller_thumbstick_axis_value; //only for button to axis mapping
}s_mapper;

typedef struct
{
  int device_type;
  int device_id;
  int button;
  int switch_back;;
}s_trigger;

typedef struct
{
  int device_up_type;
  int device_up_id;
  int up_button;
  int device_down_type;
  int device_down_id;
  int down_button;
  unsigned int dead_zone;
  e_shape shape;
  double step;
  double value;
}s_intensity;

void trigger_lookup(SDL_Event*);
void intensity_lookup(SDL_Event*);
void process_event(SDL_Event*);

#endif /* CONFIG_H_ */
