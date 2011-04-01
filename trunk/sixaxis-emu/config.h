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

#define MAX_CONTROLLERS 7
#define MAX_DEVICES 256
#define POSTPONE_COUNT 3 //to be moved to config.c later

typedef enum
{
    E_SHAPE_CIRCLE,
    E_SHAPE_RECTANGLE
}e_shape;

typedef struct
{
  int changed;
  double merge_x;
  double merge_y;
  int nb_motion;
  int postpone_wheel_up;
  int postpone_wheel_down;
  int postpone_button_x1;
  int postpone_button_x2;
  //unsigned int last_timestamp;
}s_mouse_control;

typedef struct
{
  int change;
  int send_command;
  int ts_axis[TS_MAX][TS_AXIS_MAX][2];
} s_controller;

void trigger_lookup(SDL_Event*);
void process_event(SDL_Event*);

void read_config_file(const char* file);
int read_config_dir();

#endif /* CONFIG_H_ */
