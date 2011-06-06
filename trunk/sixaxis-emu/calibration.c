/*
 * calibration.c
 *
 *  Created on: 1 juin 2011
 *      Author: Matlo
 *
 *  License: GPLv3
 */

#include <SDL/SDL.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "config.h"
#include "sixaxis.h"
#include "sdl_tools.h"
#include "config_writter.h"

#define DEFAULT_MULTIPLIER_STEP 0.01
#define EXPONENT_STEP 0.01

extern char* mouseName[MAX_DEVICES];
extern int mouseVirtualIndex[MAX_DEVICES];
extern s_mouse_control mouse_control[MAX_DEVICES];
extern s_mouse_cal mouse_cal[MAX_DEVICES][MAX_CONFIGURATIONS];
extern int refresh;
extern int mean_axis_value;
extern int display;
extern char* config_file;

int current_mouse = 0;
int current_conf = 0;
e_current_cal current_cal = NONE;

static int lctrl = 0;
static int rctrl = 0;
static int lshift = 0;
static int rshift = 0;
static int lalt = 0;
static int ralt = 0;

static const double pi = 3.14159265;

int test_time = 1000;

/*
 * Test translation acceleration.
 */
void auto_test()
{
#ifndef WIN32
  int j;
#endif
  int i, k;
  int step;

  SDL_Event mouse_evt =
  { };

  for (k = 0; k < 5; ++k)
  {
    step = 1 << k;

    for (i = 0; i < 500; i++)
    {
#ifndef WIN32
      for (j = 0; j < MAX_DEVICES && mouseName[j]; ++j)
      {
        mouse_evt.motion.xrel = step;
        mouse_evt.motion.which = j;
        mouse_evt.type = SDL_MOUSEMOTION;
        SDL_PushEvent(&mouse_evt);
      }
#else
      mouse_evt.motion.xrel = step;
      mouse_evt.motion.which = 0;
      mouse_evt.type = SDL_MOUSEMOTION;
      SDL_PushEvent(&mouse_evt);
#endif
      usleep(2000);
    }

    usleep(1000000);

    for (i = 0; i < 250; i++)
    {
#ifndef WIN32
      for (j = 0; j < MAX_DEVICES && mouseName[j]; ++j)
      {
        mouse_evt.motion.xrel = -2 * step;
        mouse_evt.motion.which = j;
        mouse_evt.type = SDL_MOUSEMOTION;
        SDL_PushEvent(&mouse_evt);
      }
#else
      mouse_evt.motion.xrel = -2 * step;
      mouse_evt.motion.which = 0;
      mouse_evt.type = SDL_MOUSEMOTION;
      SDL_PushEvent(&mouse_evt);
#endif
      usleep(2000);
    }

    usleep(1000000);
  }

  /*if(mouse_cal[current_mouse][current_conf].dzx)
   {
   state[0].user.axis[1][0] = *mouse_cal[current_mouse][current_conf].dzx;
   controller[0].send_command = 1;

   usleep(test_time*1000);

   state[0].user.axis[1][0] = 0;
   controller[0].send_command = 1;
   }*/

  /*if(mouse_cal[current_mouse][current_conf].dzy)
   {
   state[0].user.axis[1][1] = *mouse_cal[current_mouse][current_conf].dzy;
   controller[0].send_command = 1;

   usleep(test_time*1000);

   state[0].user.axis[1][1] = 0;
   controller[0].send_command = 1;

   usleep(500*1000);

   state[0].user.axis[1][1] = *mouse_cal[current_mouse][current_conf].dzy;
   controller[0].send_command = 1;

   usleep(1000*1000);

   state[0].user.axis[1][1] = 0;
   controller[0].send_command = 1;

   usleep(500*1000);

   state[0].user.axis[1][1] = - *mouse_cal[current_mouse][current_conf].dzy;
   controller[0].send_command = 1;

   usleep(test_time*1000);

   state[0].user.axis[1][1] = 0;
   controller[0].send_command = 1;

   usleep(500*1000);

   state[0].user.axis[1][1] = - *mouse_cal[current_mouse][current_conf].dzy;
   controller[0].send_command = 1;

   usleep(1000*1000);

   state[0].user.axis[1][1] = 0;
   controller[0].send_command = 1;
   }*/
}

/*
 * 
 */
void circle_test()
{
  int i, j;
  const int step = 1;

  for (i = 1; i < 360; i += step)
  {
    for (j = 0; j < DEFAULT_REFRESH_PERIOD / refresh; ++j)
    {
      mouse_control[current_mouse].merge_x += mouse_cal[current_mouse][current_conf].rd * 64 * (cos(i * 2 * pi / 360) - cos((i - 1) * 2 * pi / 360));
      mouse_control[current_mouse].merge_y += mouse_cal[current_mouse][current_conf].rd * 64 * (sin(i * 2 * pi / 360) - sin((i - 1) * 2 * pi / 360));
      mouse_control[current_mouse].change = 1;
      usleep(refresh);
    }
  }
}

/*
 * Display calibration info.
 */
void display_calibration()
{
  if (current_cal != NONE)
  {
    printf("calibrating mouse %s %d\n", mouseName[current_mouse],
        mouseVirtualIndex[current_mouse]);
    printf("calibrating conf %d\n", current_conf + 1);
    printf("multiplier_x:");
    if (mouse_cal[current_mouse][current_conf].mx)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].mx);
    }
    else
    {
      printf(" NA\n");
    }
    printf("x/y_ratio:");
    if (mouse_cal[current_mouse][current_conf].mx
        && mouse_cal[current_mouse][current_conf].my)
    {
      printf(
          " %.2f\n",
          *mouse_cal[current_mouse][current_conf].my
              / *mouse_cal[current_mouse][current_conf].mx);
    }
    else
    {
      printf(" NA\n");
    }
    printf("dead_zone_x:");
    if (mouse_cal[current_mouse][current_conf].dzx)
    {
      printf(" %d\n", *mouse_cal[current_mouse][current_conf].dzx);
    }
    else
    {
      printf(" NA\n");
    }
    printf("dead_zone_y:");
    if (mouse_cal[current_mouse][current_conf].dzy)
    {
      printf(" %d\n", *mouse_cal[current_mouse][current_conf].dzy);
    }
    else
    {
      printf(" NA\n");
    }
    printf("shape:");
    if (mouse_cal[current_mouse][current_conf].dzs)
    {
      if (*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE)
        printf(" Circle\n");
      else
        printf(" Rectangle\n");
    }
    else
    {
      printf(" NA\n");
    }
    printf("exponent_x:");
    if (mouse_cal[current_mouse][current_conf].ex)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].ex);
    }
    else
    {
      printf(" NA\n");
    }
    printf("exponent_y:");
    if (mouse_cal[current_mouse][current_conf].ey)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].ey);
    }
    else
    {
      printf(" NA\n");
    }
    printf("radius: %.2f\n", mouse_cal[current_mouse][current_conf].rd);
    printf("time: %d\n", test_time);
  }
}

/*
 * Use keys to calibrate the mouse.
 */
void calibration_key(int device_id, int sym, int down)
{
  pthread_t thread;
  pthread_attr_t thread_attr;

  switch (sym)
  {
    case SDLK_LCTRL:
      lctrl = down ? 1 : 0;
      break;
    case SDLK_RCTRL:
      rctrl = down ? 1 : 0;
      break;

    case SDLK_LSHIFT:
      lshift = down ? 1 : 0;
      break;
    case SDLK_RSHIFT:
      rshift = down ? 1 : 0;
      break;

    case SDLK_LALT:
      lalt = down ? 1 : 0;
      break;
    case SDLK_MODE:
      ralt = down ? 1 : 0;
      break;
  }

  if (lalt && ralt)
  {
    sdl_grab_toggle();
  }

  switch (sym)
  {
    case SDLK_F1:
      if (down && rctrl)
      {
        if (current_cal == NONE)
        {
          current_cal = MC;
          printf("mouse selection\n");
          display_calibration();
        }
        else
        {
          current_cal = NONE;
          if (modify_file(config_file))
          {
            printf("error writting the config file %s\n", config_file);
          }
          printf("calibration done\n");
        }
      }
      break;
    case SDLK_F2:
      if (down && rctrl)
      {
        if (current_cal == CC)
        {
          current_cal = MC;
          printf("mouse selection\n");
        }
        else if (current_cal >= MC)
        {
          current_cal = CC;
          printf("config selection\n");
        }
      }
      break;
    case SDLK_F3:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating multiplier x\n");
          current_cal = MX;
        }
      }
      break;
    case SDLK_F4:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating x/y ratio\n");
          current_cal = MY;
        }
      }
      break;
    case SDLK_F5:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating dead zone x\n");
          current_cal = DZX;
          mouse_control[current_mouse].merge_x = 1;
          mouse_control[current_mouse].merge_y = 0;
          mouse_control[current_mouse].change = 1;
        }
      }
      break;
    case SDLK_F6:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating dead zone y\n");
          current_cal = DZY;
          mouse_control[current_mouse].merge_x = 0;
          mouse_control[current_mouse].merge_y = 1;
          mouse_control[current_mouse].change = 1;
        }
      }
      break;
    case SDLK_F7:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating dead zone shape\n");
          current_cal = DZS;
          mouse_control[current_mouse].merge_x = 1;
          mouse_control[current_mouse].merge_y = 1;
          mouse_control[current_mouse].change = 1;
        }
      }
      break;
    case SDLK_F8:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating exponent x\n");
          current_cal = EX;
        }
      }
      break;
    case SDLK_F9:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("calibrating exponent y\n");
          current_cal = EY;
        }
      }
      break;
    case SDLK_F10:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          printf("adjusting circle test radius\n");
          current_cal = RD;
        }
      }
      break;
    case SDLK_F11:
      if (down && rctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, (void*) circle_test, NULL);
      }
      break;
    case SDLK_F12:
      if (down && rctrl && current_cal != NONE)
      {
        if (current_conf >= 0 && current_mouse >= 0)
        {
          current_cal = TEST;
        }
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create(&thread, &thread_attr, (void*) auto_test, NULL);
      }
      break;
  }

  if (lshift && rshift)
  {
    if (display)
    {
      display = 0;
    }
    else
    {
      display = 1;
    }
  }
}

/*
 * Use the mouse wheel to calibrate the mouse.
 */
void calibration_button(int which, int button)
{
  double ratio;

  switch (button)
  {
    case SDL_BUTTON_WHEELUP:
      switch (current_cal)
      {
        case MC:
          current_mouse += 1;
          if (!mouseName[current_mouse])
          {
            current_mouse -= 1;
          }
          break;
        case CC:
          current_conf += 1;
          if (current_conf > MAX_CONFIGURATIONS - 1)
          {
            current_conf = MAX_CONFIGURATIONS - 1;
          }
          break;
        case MX:
          if (mouse_cal[current_mouse][current_conf].mx
              && mouse_cal[current_mouse][current_conf].my)
          {
            ratio = *mouse_cal[current_mouse][current_conf].my / *mouse_cal[current_mouse][current_conf].mx;
            *mouse_cal[current_mouse][current_conf].mx += DEFAULT_MULTIPLIER_STEP;
            *mouse_cal[current_mouse][current_conf].my = *mouse_cal[current_mouse][current_conf].mx * ratio;
          }
          break;
        case MY:
          if (mouse_cal[current_mouse][current_conf].mx
              && mouse_cal[current_mouse][current_conf].my)
          {
            ratio = *mouse_cal[current_mouse][current_conf].my / *mouse_cal[current_mouse][current_conf].mx;
            ratio += DEFAULT_MULTIPLIER_STEP;
            *mouse_cal[current_mouse][current_conf].my = *mouse_cal[current_mouse][current_conf].mx * ratio;
          }
          break;
        case DZX:
          if (mouse_cal[current_mouse][current_conf].dzx)
          {
            *mouse_cal[current_mouse][current_conf].dzx += 1;
            if (*mouse_cal[current_mouse][current_conf].dzx > mean_axis_value)
            {
              *mouse_cal[current_mouse][current_conf].dzx = mean_axis_value;
            }
            mouse_control[current_mouse].merge_x = 1;
            mouse_control[current_mouse].merge_y = 0;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZY:
          if (mouse_cal[current_mouse][current_conf].dzy)
          {
            *mouse_cal[current_mouse][current_conf].dzy += 1;
            if (*mouse_cal[current_mouse][current_conf].dzy > mean_axis_value)
            {
              *mouse_cal[current_mouse][current_conf].dzy = mean_axis_value;
            }
            mouse_control[current_mouse].merge_x = 0;
            mouse_control[current_mouse].merge_y = 1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZS:
          if (mouse_cal[current_mouse][current_conf].dzs)
          {
            if (*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE)
            {
              *mouse_cal[current_mouse][current_conf].dzs = E_SHAPE_RECTANGLE;
            }
            else
            {
              *mouse_cal[current_mouse][current_conf].dzs = E_SHAPE_CIRCLE;
            }
            mouse_control[current_mouse].merge_x = 1;
            mouse_control[current_mouse].merge_y = 1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case RD:
          mouse_cal[current_mouse][current_conf].rd += 0.5;
          break;
        case EX:
          if (mouse_cal[current_mouse][current_conf].ex)
          {
            *mouse_cal[current_mouse][current_conf].ex += EXPONENT_STEP;
          }
          break;
        case EY:
          if (mouse_cal[current_mouse][current_conf].ey)
          {
            *mouse_cal[current_mouse][current_conf].ey += EXPONENT_STEP;
          }
          break;
        case TEST:
          test_time += 10;
          break;
        case NONE:
          break;
      }
      break;
    case SDL_BUTTON_WHEELDOWN:
      switch (current_cal)
      {
        case MC:
          if (current_mouse > 0)
          {
            current_mouse -= 1;
          }
          break;
        case CC:
          if (current_conf > 0)
          {
            current_conf -= 1;
          }
          break;
        case MX:
          if (mouse_cal[current_mouse][current_conf].mx && mouse_cal[current_mouse][current_conf].my)
          {
            ratio = *mouse_cal[current_mouse][current_conf].my / *mouse_cal[current_mouse][current_conf].mx;
            *mouse_cal[current_mouse][current_conf].mx -= DEFAULT_MULTIPLIER_STEP;
            *mouse_cal[current_mouse][current_conf].my = *mouse_cal[current_mouse][current_conf].mx * ratio;
          }
          break;
        case MY:
          if (mouse_cal[current_mouse][current_conf].mx && mouse_cal[current_mouse][current_conf].my)
          {
            ratio = *mouse_cal[current_mouse][current_conf].my / *mouse_cal[current_mouse][current_conf].mx;
            ratio -= DEFAULT_MULTIPLIER_STEP;
            *mouse_cal[current_mouse][current_conf].my = *mouse_cal[current_mouse][current_conf].mx * ratio;
          }
          break;
        case DZX:
          if (mouse_cal[current_mouse][current_conf].dzx)
          {
            *mouse_cal[current_mouse][current_conf].dzx -= 1;
            if (*mouse_cal[current_mouse][current_conf].dzx < 0)
            {
              *mouse_cal[current_mouse][current_conf].dzx = 0;
            }
            mouse_control[current_mouse].merge_x = -1;
            mouse_control[current_mouse].merge_y = 0;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZY:
          if (mouse_cal[current_mouse][current_conf].dzy)
          {
            *mouse_cal[current_mouse][current_conf].dzy -= 1;
            if (*mouse_cal[current_mouse][current_conf].dzy < 0)
            {
              *mouse_cal[current_mouse][current_conf].dzy = 0;
            }
            mouse_control[current_mouse].merge_x = 0;
            mouse_control[current_mouse].merge_y = -1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZS:
          if (mouse_cal[current_mouse][current_conf].dzs)
          {
            if (*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE)
            {
              *mouse_cal[current_mouse][current_conf].dzs = E_SHAPE_RECTANGLE;
            }
            else
            {
              *mouse_cal[current_mouse][current_conf].dzs = E_SHAPE_CIRCLE;
            }
            mouse_control[current_mouse].merge_x = -1;
            mouse_control[current_mouse].merge_y = -1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case RD:
          mouse_cal[current_mouse][current_conf].rd -= 0.5;
          break;
        case EX:
          if (mouse_cal[current_mouse][current_conf].ex)
          {
            *mouse_cal[current_mouse][current_conf].ex -= EXPONENT_STEP;
          }
          break;
        case EY:
          if (mouse_cal[current_mouse][current_conf].ey)
          {
            *mouse_cal[current_mouse][current_conf].ey -= EXPONENT_STEP;
          }
          break;
        case TEST:
          test_time -= 10;
          break;
        case NONE:
          break;
      }
      break;
  }

  display_calibration();
}
