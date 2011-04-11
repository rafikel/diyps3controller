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
#define MAX_CONFIGURATIONS 8
#define MAX_DEVICES 256
#define POSTPONE_COUNT 3 //to be moved to config.c later

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
  EY
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
void process_event(SDL_Event*);

void read_config_file(const char* file);
int read_config_dir();

#endif /* CONFIG_H_ */
