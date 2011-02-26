#include <sys/types.h>
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <libxml/xmlreader.h>
#include <string.h>
#include "config.h"
#include <stdlib.h>
#include "sixaxis.h"
#include <math.h>
#include "conversion.h"
#include <unistd.h>

#define X_ATTR_VALUE_KEYBOARD "keyboard"
#define X_ATTR_VALUE_MOUSE "mouse"
#define X_ATTR_VALUE_JOYSTICK "joystick"

#define X_ATTR_VALUE_BUTTON "button"
#define X_ATTR_VALUE_AXIS "axis"
#define X_ATTR_VALUE_AXIS_DOWN "axis_down"
#define X_ATTR_VALUE_AXIS_UP "axis_up"

#define MAX_CONFIGURATIONS 4
#define MAX_CONTROLS 256

#define CONFIG_DIR ".emuclient/config"

#define X_NODE_ROOT "root"
#define X_NODE_CONTROLLER "controller"
#define X_NODE_CONFIGURATION "configuration"

#define X_NODE_TRIGGER "trigger"
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

#define AXIS_X 0
#define AXIS_Y 1

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

typedef struct
{
  int nb_mappers;

  int button;
  int axis;
  int threshold;
  double multiplier;
  double exponent;
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
}s_trigger;

extern int done;
extern struct sixaxis_state state[MAX_CONTROLLERS];
extern s_controller controller[MAX_CONTROLLERS];
extern char* homedir;

/*
 * These variables are used to read the configuration.
 */

static unsigned int r_controller_id;
static unsigned int r_config_id;

static e_event_type r_event_type;
static unsigned int r_event_id;
static int r_threshold;
static unsigned int r_deadZone;
static double r_multiplier;
static double r_exponent;

static e_device_type r_device_type;
static unsigned int r_device_id;
static char* r_device_name;

/*
 * This tells what's the current config of each controller.
 */
static int current_config[MAX_CONTROLLERS];

/*
 * This lists config triggers for all controllers.
 */
static s_trigger triggers[MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * This lists controls of each controller configuration for all keyboards.
 */
static s_mapper* keyboard_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * This lists controls of each controller configuration for all mice.
 */
static s_mapper* mouse_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

static s_mapper* mouse_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * Used to tweak mouse controls.
 */
s_mouse_control mouse_control[MAX_DEVICES];

/*
 * This lists controls of each controller configuration for all joysticks.
 */
static s_mapper* joystick_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
static s_mapper* joystick_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * Clamp a value between a min and a max.
 */
static int clamp(int min, int val, int max)
{
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

/*
 * Check if current configurations of controllers need to be updated.
 */
void trigger_lookup(SDL_Event* e)
{
  int i, j;

  unsigned int device = ((SDL_KeyboardEvent*)e)->which;

  switch( e->type )
  {
    case SDL_JOYBUTTONDOWN:
    case SDL_KEYDOWN:
    case SDL_MOUSEBUTTONDOWN:
      break;
    default:
      return;
  }
  for(i=0; i<MAX_CONTROLLERS; ++i)
  {
    for(j=0; j<MAX_CONFIGURATIONS; ++j)
    {
      switch( e->type )
      {
        case SDL_JOYBUTTONDOWN:
          if(triggers[i][j].device_type != E_DEVICE_TYPE_JOYSTICK
              || device != triggers[i][j].device_id)
          {
            continue;
          }
          if(e->jbutton.button == triggers[i][j].button)
          {
            printf("controller %d is switched to configuration %d\n", i, j);
            current_config[i] = j;
          }
          break;
        case SDL_KEYDOWN:
          if(triggers[i][j].device_type != E_DEVICE_TYPE_KEYBOARD
              || device != triggers[i][j].device_id)
          {
            continue;
          }
          if(e->key.keysym.sym == triggers[i][j].button)
          {
            printf("controller %d is switched to configuration %d\n", i, j);
            current_config[i] = j;
          }
          break;
        case SDL_MOUSEBUTTONDOWN:
          if(triggers[i][j].device_type != E_DEVICE_TYPE_MOUSE
              || device != triggers[i][j].device_id)
          {
            continue;
          }
          if(e->button.button == triggers[i][j].button)
          {
            printf("controller %d is switched to configuration %d\n", i, j);
            current_config[i] = j;
          }
          break;
        default:
          break;
      }
    }
  }
}

/*
 * Specific stuff to postpone some SDL_MOUSEBUTTONUP events
 * that come too quickly after corresponding SDL_MOUSEBUTTONDOWN events.
 * If we don't do that, the PS3 will miss events.
 */
static int postpone_event(unsigned int device, SDL_Event* event)
{
  int ret = 0;
  if (event->button.button == SDL_BUTTON_WHEELUP)
  {
    if (mouse_control[device].postpone_wheel_up < POSTPONE_COUNT)
    {
      SDL_PushEvent(event);
      mouse_control[device].postpone_wheel_up++;
      ret = 1;
    }
    else
    {
      mouse_control[device].postpone_wheel_up = 0;
    }
  }
  else if (event->button.button == SDL_BUTTON_WHEELDOWN)
  {
    if (mouse_control[device].postpone_wheel_down < POSTPONE_COUNT)
    {
      SDL_PushEvent(event);
      mouse_control[device].postpone_wheel_down++;
      ret = 1;
    }
    else
    {
      mouse_control[device].postpone_wheel_down = 0;
    }
  }
  else if (event->button.button == SDL_BUTTON_X1)
  {
    if (mouse_control[device].postpone_button_x1 < POSTPONE_COUNT)
    {
      SDL_PushEvent(event);
      mouse_control[device].postpone_button_x1++;
      ret = 1;
    }
    else
    {
      mouse_control[device].postpone_button_x1 = 0;
    }
  }
  else if (event->button.button == SDL_BUTTON_X2)
  {
    if (mouse_control[device].postpone_button_x2 < POSTPONE_COUNT)
    {
      SDL_PushEvent(event);
      mouse_control[device].postpone_button_x2++;
      ret = 1;
    }
    else
    {
      mouse_control[device].postpone_button_x2 = 0;
    }
  }
  return ret;
}

extern int calibration;
extern int testing;
extern int shape;
extern double multiplier_x;
extern double multiplier_y;
extern double exponent;
extern int dead_zone_x;
extern int dead_zone_y;

static void mouse2axis(struct sixaxis_state* state, int which, double x, double y, int ts, int ts_axis, double exp, double multiplier, int dead_zone)
{
  double z = 0;
  double dz = dead_zone;

  int val = 0;

  if(which == AXIS_X)
  {
    val = x;
    if(x && y)
    {
      dz = dz*cos(atan(fabs(y/x)));
    }
  }
  else if(which == AXIS_Y)
  {
    val = y;
    if(x && y)
    {
      dz = dz*sin(atan(fabs(y/x)));
    }
  }

  if(calibration)
  {
    if(shape)
    {
      dz = dead_zone;
    }
  }
  if(testing)
  {
    z = val;
  }
  else if(val != 0)
  {
    z = multiplier * (val/fabs(val)) * pow(fabs(val), exp);
  }

  if(z > 0) state->user.axis[ts][ts_axis] = round(dz + z);
  else if(z < 0) state->user.axis[ts][ts_axis] = round(z - dz);
  else state->user.axis[ts][ts_axis] = 0;
}

extern int joystickNbButton[255];

/*
 * Updates the state table.
 * Too long function, but not hard to understand.
 */
void process_event(SDL_Event* event)
{
  s_mapper* mapper;
  int button;
  int ts;
  unsigned int ts_axis;
  unsigned int config;
  unsigned int c_id;
  unsigned int control;
  unsigned int threshold;
  double multiplier;
  double exp;
  int dead_zone;
  int value;
  unsigned int nb_controls = 0;
  SDL_Event event_jb;
  double mx;
  double my;

  /*
   * 'which' should always be at that place
   * There is no need to check the value, since it's stored as an uint8_t, and MAX_CONTROLLERS is 256.
   */
  unsigned int device = ((SDL_KeyboardEvent*)event)->which;

  for(c_id=0; c_id<MAX_CONTROLLERS; ++c_id)
  {
    config = current_config[c_id];

    nb_controls = 0;

    switch(event->type)
    {
      case SDL_JOYHATMOTION:
      event_jb.jbutton.which = event->jhat.which;
      if(event->jhat.value & SDL_HAT_UP)
      {
        event_jb.jbutton.type = SDL_JOYBUTTONDOWN;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat;
        process_event(&event_jb);
      }
      else
      {
        event_jb.jbutton.type = SDL_JOYBUTTONUP;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat;
        process_event(&event_jb);
      }
      if(event->jhat.value & SDL_HAT_RIGHT)
      {
        event_jb.jbutton.type = SDL_JOYBUTTONDOWN;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+1;
        process_event(&event_jb);
      }
      else
      {
        event_jb.jbutton.type = SDL_JOYBUTTONUP;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+1;
        process_event(&event_jb);
      }
      if(event->jhat.value & SDL_HAT_DOWN)
      {
        event_jb.jbutton.type = SDL_JOYBUTTONDOWN;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+2;
        process_event(&event_jb);
      }
      else
      {
        event_jb.jbutton.type = SDL_JOYBUTTONUP;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+2;
        process_event(&event_jb);
      }
      if(event->jhat.value & SDL_HAT_LEFT)
      {
        event_jb.jbutton.type = SDL_JOYBUTTONDOWN;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+3;
        process_event(&event_jb);
      }
      else
      {
        event_jb.jbutton.type = SDL_JOYBUTTONUP;
        event_jb.jbutton.button=joystickNbButton[event->jhat.which]+4*event->jhat.hat+3;
        process_event(&event_jb);
      }
      break;
      case SDL_JOYBUTTONDOWN:
      case SDL_JOYBUTTONUP:
      if(joystick_buttons[device][c_id][config])
      {
        nb_controls = joystick_buttons[device][c_id][config]->nb_mappers;
      }
      break;
      case SDL_JOYAXISMOTION:
      if(joystick_axis[device][c_id][config])
      {
        nb_controls = joystick_axis[device][c_id][config]->nb_mappers;
      }
      break;
      case SDL_KEYDOWN:
      case SDL_KEYUP:
      if(keyboard_buttons[device][c_id][config])
      {
        nb_controls = keyboard_buttons[device][c_id][config]->nb_mappers;
      }
      break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
      if(mouse_buttons[device][c_id][config])
      {
        nb_controls = mouse_buttons[device][c_id][config]->nb_mappers;
      }
      break;
      case SDL_MOUSEMOTION:
      if(mouse_axis[device][c_id][config])
      {
        nb_controls = mouse_axis[device][c_id][config]->nb_mappers;
      }
      break;
    }

    for(control=0; control<nb_controls; ++control)
    {
      switch(event->type)
      {
        case SDL_JOYBUTTONDOWN:
          mapper = joystick_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->jbutton.button)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 1;
            state[c_id].user.button[button].value = 255;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = joystick_buttons[device][c_id][config][control].controller_thumbstick_axis_value;
          }
          break;
        case SDL_JOYBUTTONUP:
          mapper = joystick_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->jbutton.button)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 0;
            state[c_id].user.button[button].value = 0;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = 0;
          }
          break;
        case SDL_JOYAXISMOTION:
          mapper = joystick_axis[device][c_id][config]+control;
          /*
           * Check that it's the right axis.
           */
          if(!mapper || mapper->axis != event->jaxis.axis)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Axis to button.
           */
          button = mapper->controller_button;
          threshold = mapper->threshold;
          if(button >= 0)
          {
            if(threshold > 0 && event->jaxis.value > threshold)
            {
              state[c_id].user.button[button].pressed = 1;
              state[c_id].user.button[button].value = 255;
            }
            else if(threshold < 0 && event->jaxis.value < threshold)
            {
              state[c_id].user.button[button].pressed = 1;
              state[c_id].user.button[button].value = 255;
            }
            else
            {
              state[c_id].user.button[button].pressed = 0;
              state[c_id].user.button[button].value = 0;
            }
          }
          /*
           * Axis to button axis.
           */
          button = mapper->controller_button_axis;
          multiplier = mapper->multiplier;
          exp = mapper->exponent;
          dead_zone = mapper->dead_zone;
          if(button >= 0)
          {
            value = event->jaxis.value;
            if(value)
            {
              value = value/abs(value)*multiplier*pow(abs(value), exp);
            }
            if(value > 0)
            {
              value += dead_zone;
              state[c_id].user.button[button].pressed = 1;
              state[c_id].user.button[button].value = clamp(0, value , 255);
            }
            else
            {
              state[c_id].user.button[button].pressed = 0;
              state[c_id].user.button[button].value = 0;
            }
          }
          /*
           * Axis to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          multiplier = mapper->multiplier;
          exp = mapper->exponent;
          dead_zone = mapper->dead_zone;
          if(ts >= 0)
          {
            value = event->jaxis.value;
            if(value)
            {
              value = value/abs(value)*multiplier*pow(abs(value), exp);
            }
            if(value > 0)
            {
              value += dead_zone;
            }
            else if(value < 0)
            {
              value -= dead_zone;
            }
            state[c_id].user.axis[ts][ts_axis] = clamp(-128, value , 127);
          }
          break;
        case SDL_KEYDOWN:
          mapper = keyboard_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->key.keysym.sym)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 1;
            state[c_id].user.button[button].value = 255;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = mapper->controller_thumbstick_axis_value;
          }
          break;
        case SDL_KEYUP:
          mapper = keyboard_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->key.keysym.sym)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 0;
            state[c_id].user.button[button].value = 0;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = 0;
          }
          break;
        case SDL_MOUSEMOTION:
          mapper = mouse_axis[device][c_id][config]+control;
          /*
           * Check the mouse axis.
           */
          if(!mapper || mapper->axis == 0)
          {
            value = event->motion.xrel;
          }
          else if(mapper->axis == 1)
          {
            value = event->motion.yrel;
          }
          else
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Axis to button.
           */
          button = mapper->controller_button;
          threshold = mapper->threshold;
          if(button >= 0)
          {
            //TODO: many sixaxis buttons are axis too... value may have other values than 0 and 255!
            if(threshold > 0 && value > threshold)
            {
              state[c_id].user.button[button].pressed = 1;
              state[c_id].user.button[button].value = 255;
            }
            else if(threshold < 0 && value < threshold)
            {
              state[c_id].user.button[button].pressed = 1;
              state[c_id].user.button[button].value = 255;
            }
            else
            {
              state[c_id].user.button[button].pressed = 0;
              state[c_id].user.button[button].value = 0;
            }
          }
          /*
           * Axis to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          multiplier = mapper->multiplier;
          exp = mapper->exponent;
          dead_zone = mapper->dead_zone;
          if(mouse_control[device].nb_motion)
          {
            mx = (double) mouse_control[device].merge_x / (double) mouse_control[device].nb_motion;
            my = (double) mouse_control[device].merge_y / (double) mouse_control[device].nb_motion;
          }
          else
          {
            mx = 0;
            my = 0;
          }
          if(ts >= 0)
          {
            if(mapper->axis == 0)
            {
              if(calibration)
              {
                multiplier = multiplier_x;
                exp = exponent;
                dead_zone = dead_zone_x;
              }
              mouse2axis(state+c_id, AXIS_X, mx, my, ts, ts_axis, exp, multiplier, dead_zone);
            }
            else if(mapper->axis == 1)
            {
              if(calibration)
              {
                multiplier = multiplier_y;
                exp = exponent;
                dead_zone = dead_zone_y;
              }
              mouse2axis(state+c_id, AXIS_Y, mx, my, ts, ts_axis, exp, multiplier, dead_zone);
            }
          }
          break;
        case SDL_MOUSEBUTTONDOWN:
          mapper = mouse_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->button.button)
          {
            continue;
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 1;
            state[c_id].user.button[button].value = 255;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = mapper->controller_thumbstick_axis_value;
          }
          break;
        case SDL_MOUSEBUTTONUP:
          mapper = mouse_buttons[device][c_id][config]+control;
          /*
           * Check that it's the right button.
           */
          if(!mapper || mapper->button != event->button.button)
          {
            continue;
          }
          /*
           * Check if this event needs to be postponed.
           */
          if(postpone_event(device, event))
          {
            return; //no need to do something more
          }
          controller[c_id].send_command = 1;
          /*
           * Button to button.
           */
          button = mapper->controller_button;
          if(button >= 0)
          {
            state[c_id].user.button[button].pressed = 0;
            state[c_id].user.button[button].value = 0;
          }
          /*
           * Button to axis.
           */
          ts = mapper->controller_thumbstick;
          ts_axis = mapper->controller_thumbstick_axis;
          if(ts >= 0)
          {
            state[c_id].user.axis[ts][ts_axis] = 0;
          }
          break;
      }
    }
  }
}

static inline int GetDeviceTypeProp(xmlNode * a_node)
{
  int ret = 0;
  char* type;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);

  if (!strncmp(type, X_ATTR_VALUE_KEYBOARD, strlen(X_ATTR_VALUE_KEYBOARD)))
  {
    r_device_type = E_DEVICE_TYPE_KEYBOARD;
  }
  else if (!strncmp(type, X_ATTR_VALUE_MOUSE, strlen(X_ATTR_VALUE_MOUSE)))
  {
    r_device_type = E_DEVICE_TYPE_MOUSE;
  }
  else if (!strncmp(type, X_ATTR_VALUE_JOYSTICK, strlen(X_ATTR_VALUE_JOYSTICK)))
  {
    r_device_type = E_DEVICE_TYPE_JOYSTICK;
  }
  else
  {
    r_device_type = E_DEVICE_TYPE_UNKNOWN;
  }
  return ret;
}

static inline int GetEventId(xmlNode * a_node)
{
  char* event_id = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);

  if(!event_id) return -1;

  switch(r_device_type)
  {
    case E_DEVICE_TYPE_KEYBOARD:
      r_event_id = get_key_from_buffer(event_id);
      break;
    case E_DEVICE_TYPE_JOYSTICK:
      r_event_id = atoi(event_id);
      break;
    case E_DEVICE_TYPE_MOUSE:
      r_event_id = get_mouse_event_id_from_buffer(event_id);
      break;
    default:
      break;
  }

  return 0;
}

static inline int GetIntProp(xmlNode * a_node, char* a_attr, int* a_int)
{
  char* val;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_int = atoi(val);
    return 0;
  }
  else
  {
    return -1;
  }
}

static inline int GetUnsignedIntProp(xmlNode * a_node, char* a_attr, unsigned int* a_uint)
{
  char* val;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_uint = atoi(val);
    return 0;
  }
  else
  {
    return -1;
  }
}

static inline int GetDoubleProp(xmlNode * a_node, char* a_attr, double* a_double)
{
  char* val;

  val = (char*)xmlGetProp(a_node, (xmlChar*) a_attr);

  if(val)
  {
    *a_double = strtod(val, NULL);
    return 0;
  }
  else
  {
    return -1;
  }
}

static int ProcessEventElement(xmlNode * a_node)
{
  int ret = 0;
  char* type;

  type = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_TYPE);
  if (!strncmp(type, X_ATTR_VALUE_BUTTON, strlen(X_ATTR_VALUE_BUTTON)))
  {
    r_event_type = E_EVENT_TYPE_BUTTON;
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS, strlen(X_ATTR_VALUE_AXIS)))
  {
    r_event_type = E_EVENT_TYPE_AXIS;

    ret = GetUnsignedIntProp(a_node, X_ATTR_DEADZONE, &r_deadZone);
    if(ret == -1)
    {
      r_deadZone = 0;
    }
    ret = GetDoubleProp(a_node, X_ATTR_MULTIPLIER, &r_multiplier);
    if(ret == -1)
    {
      r_multiplier = 1;
    }
    ret = GetDoubleProp(a_node, X_ATTR_EXPONENT, &r_exponent);
    if(ret == -1)
    {
      r_exponent = 1;
      ret = 0;
    }
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS_DOWN, strlen(X_ATTR_VALUE_AXIS_DOWN)))
  {
    r_event_type = E_EVENT_TYPE_AXIS_DOWN;
    ret = GetIntProp(a_node, X_ATTR_THRESHOLD, &r_threshold);
  }
  else if (!strncmp(type, X_ATTR_VALUE_AXIS_UP, strlen(X_ATTR_VALUE_AXIS_UP)))
  {
    r_event_type = E_EVENT_TYPE_AXIS_UP;
    ret = GetIntProp(a_node, X_ATTR_THRESHOLD, &r_threshold);
  }

  ret = GetEventId(a_node);

  return ret;
}

static int ProcessDeviceElement(xmlNode * a_node)
{
  int ret = 0;

  ret = GetDeviceTypeProp(a_node);

  if(ret != -1)
  {
    ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_device_id);

    r_device_name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);
  }

  return ret;
}

extern const char* joystickName[MAX_DEVICES];
extern int joystickVirtualIndex[MAX_DEVICES];

static s_mapper** get_mapper_table()
{
  s_mapper** pp_mapper = NULL;
  int i;
  switch(r_device_type)
  {
    case E_DEVICE_TYPE_KEYBOARD:
      pp_mapper = &(keyboard_buttons[r_device_id][r_controller_id][r_config_id]);
      break;
    case E_DEVICE_TYPE_MOUSE:
      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          pp_mapper = &(mouse_buttons[r_device_id][r_controller_id][r_config_id]);
          break;
        case E_EVENT_TYPE_AXIS:
          pp_mapper = &(mouse_axis[r_device_id][r_controller_id][r_config_id]);
          break;
        default:
          break;
      }
      break;
    case E_DEVICE_TYPE_JOYSTICK:
      for(i=0; i<MAX_DEVICES && joystickName[i]; ++i)
      {
        if(!strcmp(r_device_name, joystickName[i]))
        {
          if(r_device_id == joystickVirtualIndex[i])
          {
            r_device_id = i;
            break;
          }
        }
      }
      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          pp_mapper = &(joystick_buttons[r_device_id][r_controller_id][r_config_id]);
          break;
        case E_EVENT_TYPE_AXIS:
          pp_mapper = &(joystick_axis[r_device_id][r_controller_id][r_config_id]);
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return pp_mapper;
}

static void allocate_mapper(s_mapper** pp_mapper)
{
  if(*pp_mapper)
  {
    *pp_mapper = realloc(*pp_mapper, ((*pp_mapper)->nb_mappers+1)*sizeof(s_mapper));
    memset(*pp_mapper+(*pp_mapper)->nb_mappers, 0x00, sizeof(s_mapper));
  }
  else
  {
    *pp_mapper = calloc(1, sizeof(s_mapper));
  }
  (*pp_mapper)->nb_mappers++;
}

static int ProcessAxisElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  s_mapper** pp_mapper = NULL;
  s_mapper* p_mapper = NULL;
  s_axis_index aindex;
  char* aid;

  aid = (char*)xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);

  aindex = get_axis_index_from_name(aid);

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
      {
        ret = ProcessDeviceElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing device element");
    ret = -1;
  }

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
      {
        ret = ProcessEventElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing event element");
    ret = -1;
  }

  if(ret != -1)
  {
    pp_mapper = get_mapper_table();

    if(pp_mapper)
    {
      allocate_mapper(pp_mapper);

      p_mapper = *pp_mapper;

      p_mapper = p_mapper+p_mapper->nb_mappers-1;

      p_mapper->controller_button = -1;
      p_mapper->controller_button_axis = aindex.baindex;
      p_mapper->controller_thumbstick_axis_value = aindex.tavalue;
      p_mapper->controller_thumbstick = aindex.tindex;
      p_mapper->controller_thumbstick_axis = aindex.taindex;

      switch(r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          p_mapper->button = r_event_id;
          break;
        case E_EVENT_TYPE_AXIS:
          p_mapper->axis = r_event_id;
          p_mapper->dead_zone = r_deadZone;
          p_mapper->multiplier = r_multiplier;
          p_mapper->exponent = r_exponent;
          break;
        default:
          break;
      }
    }
  }

  return ret;
}

static int ProcessButtonElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;
  char* bid;
  e_sixaxis_button_index bindex;
  s_mapper* p_mapper = NULL;
  s_mapper** pp_mapper = NULL;

  bid = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_ID);

  ret = get_button_index_from_name(bid);

  if(ret != -1)
  {
    bindex = ret;
  }

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_DEVICE))
      {
        ret = ProcessDeviceElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing device element");
    ret = -1;
  }

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_EVENT))
      {
        ret = ProcessEventElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing event element");
    ret = -1;
  }

  if(ret != -1)
  {
    pp_mapper = get_mapper_table();

    if(pp_mapper)
    {
      allocate_mapper(pp_mapper);

      p_mapper = *pp_mapper;

      p_mapper = p_mapper + p_mapper->nb_mappers - 1;

      p_mapper->controller_button = bindex;
      p_mapper->controller_thumbstick = -1;

      switch (r_event_type)
      {
        case E_EVENT_TYPE_BUTTON:
          p_mapper->button = r_event_id;
          break;
        case E_EVENT_TYPE_AXIS_DOWN:
        case E_EVENT_TYPE_AXIS_UP:
          p_mapper->axis = r_event_id;
          p_mapper->threshold = r_threshold;
          break;
        default:
          break;
      }
    }
  }

  return ret;
}

static int ProcessAxisMapElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS))
      {
        ret = ProcessAxisElement(cur_node);
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }
  return ret;
}

static int ProcessButtonMapElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON))
      {
        ret = ProcessButtonElement(cur_node);
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }
  return ret;
}

static int ProcessTriggerElement(xmlNode * a_node)
{
  int ret = 0;
  char* device_name;
  char* event_id;

  ret = GetDeviceTypeProp(a_node);

  if(ret != -1)
  {
    //TODO: manage device name
    device_name = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_NAME);

    ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_device_id);

    if(ret != -1)
    {
      if(r_device_type == E_DEVICE_TYPE_KEYBOARD)
      {
        event_id = (char*) xmlGetProp(a_node, (xmlChar*) X_ATTR_BUTTON_ID);
        r_event_id = get_key_from_buffer(event_id);
      }
    }
  }

  if(ret != -1)
  {
    triggers[r_controller_id][r_config_id].button = r_event_id;
    triggers[r_controller_id][r_config_id].device_id = r_device_id;
    triggers[r_controller_id][r_config_id].device_type = r_device_type;
  }

  return ret;
}

static int ProcessConfigurationElement(xmlNode * a_node)
{
  int ret = 0;
  xmlNode* cur_node = NULL;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_config_id);

  if(ret != -1)
  {
	  r_config_id--;

    if (r_config_id >= MAX_CONFIGURATIONS || r_config_id < 0)
    {
      printf("bad configuration id: %d\n", r_config_id);
      ret = -1;
    }
  }

  cur_node = a_node->children;

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_TRIGGER))
      {
        ret = ProcessTriggerElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing trigger element");
    ret = -1;
  }

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_BUTTON_MAP))
      {
        ret = ProcessButtonMapElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing button_map element");
    ret = -1;
  }

  for (cur_node = cur_node->next; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_AXIS_MAP))
      {
        ret = ProcessAxisMapElement(cur_node);
        break;
      }
      else
      {
        printf("bad element name: %s", cur_node->name);
        ret = -1;
      }
    }
  }

  if (!cur_node)
  {
    printf("missing axis_map element");
    ret = -1;
  }

  return ret;
}

static int ProcessControllerElement(xmlNode * a_node)
{
  xmlNode* cur_node = NULL;
  int ret = 0;

  ret = GetUnsignedIntProp(a_node, X_ATTR_ID, &r_controller_id);

  if(ret != -1)
  {
	r_controller_id--;

    if (r_controller_id >= MAX_CONTROLLERS || r_controller_id < 0)
    {
      printf("bad controller id: %d\n", r_controller_id);
      ret = -1;
    }
  }

  for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
  {
    if (cur_node->type == XML_ELEMENT_NODE)
    {
      if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONFIGURATION))
      {
        ret = ProcessConfigurationElement(cur_node);
      }
      else
      {
        ret = -1;
        printf("bad element name: %s\n", cur_node->name);
      }
    }
  }
  return ret;
}

static int ProcessRootElement(xmlNode * a_node)
{
  xmlNode *cur_node = NULL;
  int ret = 0;

  if (a_node->type == XML_ELEMENT_NODE)
  {
    if (xmlStrEqual(a_node->name, (xmlChar*) X_NODE_ROOT))
    {
      for (cur_node = a_node->children; cur_node && ret != -1; cur_node = cur_node->next)
      {
        if (cur_node->type == XML_ELEMENT_NODE)
        {
          if (xmlStrEqual(cur_node->name, (xmlChar*) X_NODE_CONTROLLER))
          {
            ret = ProcessControllerElement(cur_node);
          }
          else
          {
            ret = -1;
            printf("bad element name: %s\n", cur_node->name);
          }
        }
      }
    }
    else
    {
      ret = -1;
      printf("bad element name: %s\n", a_node->name);
    }
  }
  return ret;
}

/*
 * This function loads a config file.
 */
static int read_file(char* file_path)
{
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;
  int ret = 0;

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*parse the file and get the DOM */
  doc = xmlReadFile(file_path, NULL, 0);

  if (doc != NULL)
  {
    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    if(root_element != NULL)
    {
      ret = ProcessRootElement(root_element);
    }
    else
    {
      ret = -1;
      printf("error: no root element\n");
    }
  }
  else
  {
    ret = -1;
    printf("error: could not parse file %s\n", file_path);
  }

  /*free the document */
  xmlFreeDoc(doc);

  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();

  return ret;

}

/*
 * This function loads a config file.
 */
void read_config_file(const char* file)
{
  char file_path[PATH_MAX];

  snprintf(file_path, sizeof(file_path), "%s/%s/%s", homedir, CONFIG_DIR, file);

  if(read_file(file_path) == -1)
  {
    printf("Bad config file: %s\n", file_path);
    exit(-1);
  }
}

/*
 * This function reads all filenames in the config directory
 * and ask the user which one he wants to load.
 */
int read_config_dir()
{
  DIR *dirp;
  struct dirent *d;
  char file_path[PATH_MAX];
  unsigned int i;
  char line[LINE_MAX];
  unsigned int selected = 0;
  unsigned int nb_filenames = 0;
  char** filenames = NULL;

  snprintf(file_path, sizeof(file_path), "%s/%s", homedir, CONFIG_DIR);
  dirp = opendir(file_path);
  if (dirp == NULL)
  {
    printf("Warning: can't open config directory %s\n", file_path);
    exit(-1);
  }

  while ((d = readdir(dirp)))
  {
    if (d->d_type == DT_REG)
    {
      nb_filenames++;
      filenames = realloc(filenames, nb_filenames*sizeof(char*));
      filenames[nb_filenames-1] = calloc(strlen(d->d_name)+1, sizeof(char));
      strncpy(filenames[nb_filenames-1], d->d_name, strlen(d->d_name));
    }
  }

  closedir(dirp);

  if(!nb_filenames)
  {
    printf("error: no config file found\n");
    return 0;
  }

  printf("Please choose the config file to load:\n");
  for(i=0; i<nb_filenames; ++i)
  {
    printf("\t%d - %s\n", i, filenames[i]);
  }

  fgets(line, sizeof(line), stdin);
  while(sscanf(line, "%d\n", &selected) != 1 || selected >= nb_filenames)
  {
    printf("Wrong value!");
    fgets(line, sizeof(line), stdin);
  }

  printf("Selected config file: %s\n", filenames[selected]);

  read_config_file(filenames[selected]);

  for(i=0; i<nb_filenames; ++i)
  {
    free(filenames[i]);
  }
  free(filenames);

  return 1;
}
