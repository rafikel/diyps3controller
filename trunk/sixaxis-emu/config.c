#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include <stdlib.h>
#include "sixaxis.h"
#include <math.h>
#include "conversion.h"
#include <unistd.h>

extern struct sixaxis_state state[MAX_CONTROLLERS];
extern s_controller controller[MAX_CONTROLLERS];
extern int display;

extern int joystickNbButton[255];
extern int joystickSixaxis[MAX_DEVICES];

extern int current_mouse;
extern e_current_cal current_cal;

/*
 * This tells what's the current config of each controller.
 */
static int current_config[MAX_CONTROLLERS];

/*
 * This tells what's the previous config of each controller.
 * Hackish way to switch back to previous config.
 * Only used with left mouse clic.
 */
static int previous_config[MAX_CONTROLLERS];

/*
 * This lists config triggers for all controllers.
 */
s_trigger triggers[MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * This lists controller stick intensity modifiers.
 */
s_intensity left_intensity[MAX_CONTROLLERS][MAX_CONFIGURATIONS];
s_intensity right_intensity[MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * This lists controls of each controller configuration for all keyboards.
 */
s_mapper* keyboard_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * This lists controls of each controller configuration for all mice.
 */
s_mapper* mouse_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

s_mapper* mouse_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

/*
 * Used to tweak mouse controls.
 */
s_mouse_control mouse_control[MAX_DEVICES];

/*
 * Used to calibrate mouse controls.
 */
s_mouse_cal mouse_cal[MAX_DEVICES][MAX_CONFIGURATIONS] = {};

/*
 * This lists controls of each controller configuration for all joysticks.
 */
s_mapper* joystick_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
s_mapper* joystick_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

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
  int device_type;
  int button;
  int up = 0;
  unsigned int device_id = ((SDL_KeyboardEvent*)e)->which;

  switch( e->type )
  {
    case SDL_JOYBUTTONUP:
      up = 1;
    case SDL_JOYBUTTONDOWN:
      device_type = E_DEVICE_TYPE_JOYSTICK;
      button = e->jbutton.button;
      break;
    case SDL_KEYUP:
      up = 1;
    case SDL_KEYDOWN:
      device_type = E_DEVICE_TYPE_KEYBOARD;
      button = e->key.keysym.sym;
      break;
    case SDL_MOUSEBUTTONUP:
      up = 1;
    case SDL_MOUSEBUTTONDOWN:
      device_type = E_DEVICE_TYPE_MOUSE;
      button = e->button.button;
      break;
    default:
      return;
  }

  for(i=0; i<MAX_CONTROLLERS; ++i)
  {
    for(j=0; j<MAX_CONFIGURATIONS; ++j)
    {
      if (triggers[i][j].device_type != device_type || device_id
          != triggers[i][j].device_id)
      {
        continue;
      }
      if (button == triggers[i][j].button)
      {
        if (display)
        {
          printf("controller %d is switched from configuration %d", i, current_config[i]);
        }
        if (!up)
        {
          previous_config[i] = current_config[i];
          current_config[i] = j;
        }
        else
        {
          current_config[i] = previous_config[i];
        }
        if (display)
        {
          printf(" to %d\n", current_config[i]);
        }
      }
    }
  }
}

/*
 * Check if stick intensities need to be updated.
 */
void intensity_lookup(SDL_Event* e)
{
  int i, j;
  int device_type;
  int button;
  unsigned int device_id = ((SDL_KeyboardEvent*)e)->which;

  switch( e->type )
  {
    case SDL_JOYBUTTONDOWN:
      device_type = E_DEVICE_TYPE_JOYSTICK;
      button = e->jbutton.button;
      break;
    case SDL_KEYDOWN:
      device_type = E_DEVICE_TYPE_KEYBOARD;
      button = e->key.keysym.sym;
      break;
    case SDL_MOUSEBUTTONDOWN:
      device_type = E_DEVICE_TYPE_MOUSE;
      button = e->button.button;
      break;
    default:
      return;
  }

  for(i=0; i<MAX_CONTROLLERS; ++i)
  {
    for(j=0; j<MAX_CONFIGURATIONS; ++j)
    {
      if (left_intensity[i][j].device_type != device_type || device_id
          != left_intensity[i][j].device_id)
      {
        continue;
      }
      if (button == left_intensity[i][j].button)
      {
        left_intensity[i][j].value += left_intensity[i][j].step;
        if(left_intensity[i][j].value > 127) left_intensity[i][j].value = left_intensity[i][j].step;

        if (display)
        {
          printf("controller %d configuration %d left intensity: %.0f\n", i, j, left_intensity[i][j].value);
        }

        /*
         * We have to update current stick intensity according to the new value.
         */
        if(current_config[i] == j)
        {
          if(state[i].user.axis[0][0] && state[i].user.axis[0][1])
          {
            state[i].user.axis[0][0] = left_intensity[i][j].value;
            state[i].user.axis[0][1] = left_intensity[i][j].value;
            controller[i].send_command = 1;
          }
          else if(state[i].user.axis[0][0])
          {
            state[i].user.axis[0][0] = left_intensity[i][j].value;
            controller[i].send_command = 1;

          }
          else if(state[i].user.axis[0][1])
          {
            state[i].user.axis[0][1] = left_intensity[i][j].value;
            controller[i].send_command = 1;
          }
        }
      }
      if (button == right_intensity[i][j].button)
      {
        right_intensity[i][j].value += right_intensity[i][j].step;
        if(right_intensity[i][j].value > 127) right_intensity[i][j].value = right_intensity[i][j].step;

        if (display)
        {
          printf("controller %d configuration %d right intensity: %.0f\n", i, j, right_intensity[i][j].value);
        }

        /*
         * We have to update current stick intensity according to the new value.
         */
        if(current_config[i] == j)
        {
          if(state[i].user.axis[1][0] && state[i].user.axis[1][1])
          {
            state[i].user.axis[1][0] = right_intensity[i][j].value;
            state[i].user.axis[1][1] = right_intensity[i][j].value;
            controller[i].send_command = 1;
          }
          else if(state[i].user.axis[1][0])
          {
            state[i].user.axis[1][0] = right_intensity[i][j].value;
            controller[i].send_command = 1;
          }
          else if(state[i].user.axis[1][1])
          {
            state[i].user.axis[1][1] = right_intensity[i][j].value;
            controller[i].send_command = 1;
          }
        }
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

static void mouse2axis(int device, struct sixaxis_state* state, int which, double x, double y, int ts, int ts_axis, double exp, double multiplier, int dead_zone, e_shape shape)
{
  double z = 0;
  double dz = dead_zone;

  int val = 0;

  if(which == AXIS_X)
  {
    val = x;
    if(x && y && shape == E_SHAPE_CIRCLE)
    {
      dz = dz*cos(atan(fabs(y/x)));
    }
    if(device == current_mouse && (current_cal == DZX || current_cal == DZS))
    {
      if(val > 0)
      {
        state->user.axis[ts][ts_axis] = dz;
      }
      else
      {
        state->user.axis[ts][ts_axis] = -dz;
      }
      return;
    }
  }
  else if(which == AXIS_Y)
  {
    val = y;
    if(x && y && shape == E_SHAPE_CIRCLE)
    {
      dz = dz*sin(atan(fabs(y/x)));
    }
    if(device == current_mouse && (current_cal == DZY || current_cal == DZS))
    {
      if(val > 0)
      {
        state->user.axis[ts][ts_axis] = dz;
      }
      else
      {
        state->user.axis[ts][ts_axis] = -dz;
      }
      return;
    }
  }

  if(val != 0)
  {
    z = multiplier * (val/fabs(val)) * pow(fabs(val), exp);
  }

  if(z > 0) state->user.axis[ts][ts_axis] = round(dz + z);
  else if(z < 0) state->user.axis[ts][ts_axis] = round(z - dz);
  else state->user.axis[ts][ts_axis] = 0;
}

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
  e_shape shape;
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
      if(joystickSixaxis[device] && event->jaxis.axis > 3)
      {
        event->jaxis.value = (event->jaxis.value + 32767) / 2;
      }
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
            if(ts == 0)
            {
              if(mapper->controller_thumbstick_axis_value < 0)
              {
                state[c_id].user.axis[ts][ts_axis] = - left_intensity[c_id][config].value;
              }
              else
              {
                state[c_id].user.axis[ts][ts_axis] = left_intensity[c_id][config].value;
              }
            }
            else
            {
              if(mapper->controller_thumbstick_axis_value < 0)
              {
                state[c_id].user.axis[ts][ts_axis] = - right_intensity[c_id][config].value;
              }
              else
              {
                state[c_id].user.axis[ts][ts_axis] = right_intensity[c_id][config].value;
              }

            }
            /*
             * Specific code for issue 15.
             */
            if(mapper->controller_thumbstick_axis_value == 127)
            {
              controller[c_id].ts_axis[ts][ts_axis][0] = 1;
            }
            else if(mapper->controller_thumbstick_axis_value == -127)
            {
              controller[c_id].ts_axis[ts][ts_axis][1] = 1;
            }
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
            /*
             * Specific code for issue 15.
             */
            if(mapper->controller_thumbstick_axis_value == 127)
            {
              controller[c_id].ts_axis[ts][ts_axis][0] = 0;
              if(controller[c_id].ts_axis[ts][ts_axis][1] == 1)
              {
                state[c_id].user.axis[ts][ts_axis] = -mapper->controller_thumbstick_axis_value;
              }
            }
            else if(mapper->controller_thumbstick_axis_value == -127)
            {
              controller[c_id].ts_axis[ts][ts_axis][1] = 0;
              if(controller[c_id].ts_axis[ts][ts_axis][0] == 1)
              {
                state[c_id].user.axis[ts][ts_axis] = -mapper->controller_thumbstick_axis_value;
              }
            }
          }
          break;
        case SDL_MOUSEMOTION:
          mapper = mouse_axis[device][c_id][config]+control;
          /*
           * Check the mouse axis.
           */
          if(!mapper)
          {
            continue;
          }
          else if (mapper->axis == 0)
          {
            value = event->motion.xrel;
          }
          else if(mapper->axis == 1)
          {
            value = event->motion.yrel;
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
          shape = mapper->shape;
          if(mouse_control[device].change)
          {
            mx = mouse_control[device].merge_x;
            my = mouse_control[device].merge_y;
          }
          else
          {
            mx = 0;
            my = 0;
          }
          if(ts >= 0)
          {
            mouse2axis(device, state+c_id, mapper->axis, mx, my, ts, ts_axis, exp, multiplier, dead_zone, shape);
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
