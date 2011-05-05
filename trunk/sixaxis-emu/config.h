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
#include <libxml/xmlreader.h>

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
#define X_NODE_LEFT_INTENSITY "left_intensity"
#define X_NODE_RIGHT_INTENSITY "right_intensity"
#define X_NODE_BUTTON_MAP "button_map"
#define X_NODE_AXIS_MAP "axis_map"
#define X_NODE_DEVICE "device"
#define X_NODE_EVENT "event"
#define X_NODE_AXIS "axis"
#define X_NODE_BUTTON "button"

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

void trigger_lookup(SDL_Event*);
void intensity_lookup(SDL_Event*);
void process_event(SDL_Event*);

void read_config_file(const char*);
int read_config_dir();

int GetIntProp(xmlNode*, char*, int*);
int GetUnsignedIntProp(xmlNode*, char*, unsigned int*);
int GetDoubleProp(xmlNode*, char*, double*);

#endif /* CONFIG_H_ */
