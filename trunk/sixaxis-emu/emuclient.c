/* Sixaxis emulator

 Copyright (c) 2010 Mathieu Laurendeau

 Copyright (c) 2009 Jim Paris <jim@jtan.com>
 License: GPLv3
 */

#ifndef WIN32
#include <termio.h>
#include <sys/ioctl.h>
#include <err.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pwd.h>
#include "tcp_con.h"
#include <sys/resource.h>
#include <sched.h>
#else
#include <windows.h>
#define sleep Sleep
#endif
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include "sdl_tools.h"
#include "sixaxis.h"
#include "dump.h"
#include "macros.h"
#include "config.h"
#include "config_writter.h"
#include "config_reader.h"
#include <sys/time.h>
#include "calibration.h"
#include <libxml/parser.h>
#include "serial_con.h"

#define EVENT_BUFFER_SIZE 256
#define DEFAULT_POSTPONE_COUNT 3
#define DEFAULT_MAX_AXIS_VALUE 255
#define DEFAULT_AXIS_SCALE 1

#ifndef WIN32
char* homedir = "";
//#else
//char* ip = "";
#endif

#ifdef WIN32
static void err(int eval, const char *fmt)
{
  fprintf(stderr, fmt);
  exit(eval);
}
#endif

char* config_file = NULL;

char* portname = NULL;

int refresh = DEFAULT_REFRESH_PERIOD; //µs
int postpone_count = DEFAULT_POSTPONE_COUNT;
int max_axis_value = DEFAULT_MAX_AXIS_VALUE;
int mean_axis_value = DEFAULT_MAX_AXIS_VALUE / 2;
double axis_scale = DEFAULT_AXIS_SCALE;
double frequency_scale;
int subpos = 0;

int serial = 0;
int done = 0;
int display = 0;

struct sixaxis_state state[MAX_CONTROLLERS];
s_controller controller[MAX_CONTROLLERS] =
{ };

extern s_mouse_control mouse_control[MAX_DEVICES];
extern s_mouse_cal mouse_cal[MAX_DEVICES][MAX_CONFIGURATIONS];

extern char* mouseName[MAX_DEVICES];
extern int mouseVirtualIndex[MAX_DEVICES];

extern e_current_cal current_cal;
extern int current_mouse;

extern int dpi;

int main(int argc, char *argv[])
{
  int grab = 1;
  SDL_Event events[EVENT_BUFFER_SIZE];
  SDL_Event* event;
  SDL_Event mouse_evt =
  { };
  int i;
  int num_evt;
  int read = 0;
  struct timeval t0, t1;
  int time_to_sleep;

#ifndef WIN32
  /*
   * Set highest priority & scheduler policy.
   */
  struct sched_param p = {.sched_priority = 99};

  sched_setscheduler(0, SCHED_FIFO, &p);
  //setpriority(PRIO_PROCESS, getpid(), -20); only useful with SCHED_OTHER

  setlinebuf(stdout);
  homedir = getpwuid(getuid())->pw_dir;

  system("test -d ~/.emuclient || cp -r /etc/emuclient ~/.emuclient");
#else
  SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif

  for (i = 1; i < argc; ++i)
  {
    if (!strcmp(argv[i], "--nograb"))
    {
      grab = 0;
    }
    else if (!strcmp(argv[i], "--config") && i < argc)
    {
      config_file = argv[++i];
      read = 1;
    }
    else if (!strcmp(argv[i], "--port") && i < argc)
    {
      portname = argv[++i];
    }
    else if (!strcmp(argv[i], "--status"))
    {
      display = 1;
    }
    else if (!strcmp(argv[i], "--refresh"))
    {
      refresh = atoi(argv[++i]) * 1000;
      postpone_count = 3 * DEFAULT_REFRESH_PERIOD / refresh;
    }
    else if (!strcmp(argv[i], "--precision"))
    {
      max_axis_value = (1 << atoi(argv[++i])) - 1;
      mean_axis_value = max_axis_value / 2;
    }
    else if (!strcmp(argv[i], "--serial"))
    {
      serial = 1;
    }
    else if (!strcmp(argv[i], "--subpos"))
    {
      subpos = 1;
    }
    else if (!strcmp(argv[i], "--dpi"))
    {
      dpi = atoi(argv[++i]);
    }
//#ifdef WIN32
//    else if (!strcmp(argv[i], "--ip") && i < argc)
//    {
//      ip = argv[++i];
//    }
//#endif
  }

#ifdef WIN32
  if (!portname)
  {
    err(1, "no serial port specified!\n");
  }
  /*
   * Force precision to 16bits
   */
  max_axis_value = (1 << 16) - 1;
  mean_axis_value = max_axis_value / 2;
#endif

  if (display == 1)
  {
    printf("max_axis_value: %d\n", max_axis_value);//needed by sixstatus...
  }

  axis_scale = (double) max_axis_value / DEFAULT_MAX_AXIS_VALUE;
  frequency_scale = (double) DEFAULT_REFRESH_PERIOD / refresh;

  initialize_macros();

  for (i = 0; i < MAX_CONTROLLERS; ++i)
  {
    sixaxis_init(state + i);
    memset(controller + i, 0x00, sizeof(s_controller));
  }

  if (!sdl_initialize())
  {
    err(1, "can't init sdl");
  }

  if(grab)
  {
    sleep(1);
    sdl_grab();
  }

  if (read == 1)
  {
    read_config_file(config_file);
    sdl_release_unused();
  }

#ifndef WIN32
  if(serial)
  {
    if (serial_connect(portname) < 0)
    {
      err(1, "serial_connect");
    }
  }
  else if(tcp_connect() < 0)
  {
    err(1, "tcp_connect");
  }
#else
  if (serial_connect(portname) < 0)
  {
    err(1, "serial_connect");
  }
#endif

  done = 0;
  while (!done)
  {
    gettimeofday(&t0, NULL);

    SDL_PumpEvents();
    num_evt = SDL_PeepEvents(events, sizeof(events) / sizeof(events[0]),
        SDL_GETEVENT, SDL_ALLEVENTS);

    if (num_evt == EVENT_BUFFER_SIZE)
    {
      printf("buffer too small!!!\n");
    }

    for (event = events; event < events + num_evt; ++event)
    {
      if (event->type != SDL_MOUSEMOTION)
      {
        //if calibration is on, all mouse wheel events are skipped
        if (current_cal == NONE || event->type != SDL_MOUSEBUTTONDOWN
            || (event->button.button != SDL_BUTTON_WHEELDOWN && event->button.button != SDL_BUTTON_WHEELUP))
        {
          process_event(event);
        }
      }
      else
      {
        mouse_control[event->motion.which].merge_x += event->motion.xrel;
        mouse_control[event->motion.which].merge_y += event->motion.yrel;
        mouse_control[event->motion.which].change = 1;
      }

      trigger_lookup(event);
      intensity_lookup(event);

      switch (event->type)
      {
        case SDL_QUIT:
          done = 1;
          break;
        case SDL_KEYDOWN:
          calibration_key(event->key.which, event->key.keysym.sym, 1);
          macro_lookup(event->key.which, event->key.keysym.sym);
          if(event->key.keysym.sym == SDLK_ESCAPE)
          {
            done = 1;
          }
          break;
        case SDL_KEYUP:
          calibration_key(event->key.which, event->key.keysym.sym, 0);
          break;
        case SDL_MOUSEBUTTONDOWN:
          calibration_button(event->button.which, event->button.button);
          break;
      }
    }

    config_activation();

    /*
     * Process a single (merged) motion event for each mouse.
     */
    for (i = 0; i < MAX_DEVICES; ++i)
    {
      if (mouse_control[i].changed || mouse_control[i].change)
      {
        if (subpos)
        {
          /*
           * Add the residual motion vector from the last iteration.
           */
          mouse_control[i].merge_x += mouse_control[i].residue_x;
          mouse_control[i].merge_y += mouse_control[i].residue_y;
          /*
           * If no motion was received this iteration, the residual motion vector from the last iteration is reset.
           */
          if (!mouse_control[i].change)
          {
            mouse_control[i].residue_x = 0;
            mouse_control[i].residue_y = 0;
          }
        }
        mouse_evt.motion.which = i;
        mouse_evt.type = SDL_MOUSEMOTION;
        process_event(&mouse_evt);
      }
      mouse_control[i].merge_x = 0;
      mouse_control[i].merge_y = 0;
      mouse_control[i].changed = mouse_control[i].change;
      mouse_control[i].change = 0;
      if (i == current_mouse && (current_cal == DZX || current_cal == DZY || current_cal == DZS))
      {
        mouse_control[i].changed = 0;
      }
    }

#ifndef WIN32
    if(serial)
    {
      serial_send();
    }
    else
    {
      tcp_send(serial);
    }
#else
    serial_send();
#endif

#ifdef WIN32
    /*
     * There is no setlinebuf(stdout) in windows.
     */
    if(display)
    {
      fflush(stdout);
    }
#endif

    gettimeofday(&t1, NULL);

    time_to_sleep = refresh - ((t1.tv_sec * 1000000 + t1.tv_usec) - (t0.tv_sec * 1000000 + t0.tv_usec));

    if (time_to_sleep > 0)
    {
      usleep(time_to_sleep);
    }
    else

    {
      printf("processing time higher than %dms!!\n", refresh / 1000);
    }
  }

  printf("Exiting\n");

  free_macros();
  sdl_quit();
#ifdef WIN32
  serial_close();
#endif

  xmlCleanupParser();

  return 0;
}
