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
#else
#include <winsock2.h>
#define MSG_DONTWAIT 0
#endif
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL/SDL.h>
#include "sixaxis.h"
#include "dump.h"
#include "macros.h"
#include "config.h"
#include <math.h>

#include <pthread.h>

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define DEFAULT_DEAD_ZONE_X 18
#define DEFAULT_DEAD_ZONE_Y 20
#define DEFAULT_MULTIPLIER_X 4
#define DEFAULT_MULTIPLIER_Y 9
#define DEFAULT_EXPONENT 1
#define MULTIPLIER_STEP 0.25
#define EXPONENT_STEP 0.1
#define REFRESH_PERIOD 10000 //=10ms
#define EVENT_BUFFER_SIZE 32

static int debug = 0;
int done = 0;
static double multiplier_x = DEFAULT_MULTIPLIER_X;
static double multiplier_y = DEFAULT_MULTIPLIER_Y;
static double exponent = DEFAULT_EXPONENT;
static int dead_zone_x = DEFAULT_DEAD_ZONE_X;
static int dead_zone_y = DEFAULT_DEAD_ZONE_Y;
int calibration = 0;
static int lctrl = 0;
static int rctrl = 0;

SDL_Surface *screen = NULL;

struct sixaxis_state state[MAX_CONTROLLERS];
int (*assemble)(uint8_t *buf, int len, struct sixaxis_state *state);
static int sockfd[MAX_CONTROLLERS];
s_controller controller[MAX_CONTROLLERS];

#ifdef WIN32
static void err(int eval, const char *fmt)
{
    fprintf(stderr, fmt);
    exit(eval);
}

static void warn(const char *fmt)
{
    fprintf(stderr, fmt);
}
#endif

int initialize(int width, int height, const char *title)
{
  int i = 0;

  /* Init SDL */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    return 0;
  }

#ifdef WIN32
  /* enable stdout and stderr */
  freopen( "CON", "w", stdout );
  freopen( "CON", "w", stderr );
#endif

  SDL_WM_SetCaption(title, title);

  /* Init video */
  screen = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_ANYFORMAT);
  if (screen == NULL)
  {
    fprintf(stderr, "Unable to create video surface: %s\n", SDL_GetError());
    return 0;
  }

  SDL_WM_GrabInput(SDL_GRAB_ON);
  SDL_ShowCursor(SDL_DISABLE);

  while(SDL_JoystickOpen(i))
  {
    i++;
  }

  return 1;
}

#define TCP_PORT 21313

int tcpconnect(void)
{
    int fd;
    int i;
    int ret = -1;
    struct sockaddr_in addr;

    for(i=0; i<MAX_CONTROLLERS; ++i)
    {

#ifdef WIN32
      WSADATA wsadata;

      if (WSAStartup(MAKEWORD(1,1), &wsadata) == SOCKET_ERROR)
        err(1, "WSAStartup");

      if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err(1, "socket");
#else
      if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        err(1, "socket");
#endif
      memset(&addr, 0, sizeof(addr));
      addr.sin_family = AF_INET;
      addr.sin_port = htons(TCP_PORT+i);
      addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //inet_addr("192.168.56.101");
      if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
      {
        fd = 0;
        warn("can't connect to port %d", i+TCP_PORT);
      }
      else
      {
        ret = 1;
      }

#ifdef WIN32
      // Set the socket I/O mode; iMode = 0 for blocking; iMode != 0 for non-blocking
      int iMode = 1;
      ioctlsocket(fd, FIONBIO, (u_long FAR*) &iMode);
#endif

      sockfd[i] = fd;

    }

    return ret;
}

void move_x(int x)
{
    state[0].user.axis[0][0] = x;
}

void move_y(int y)
{
    state[0].user.axis[0][1] = y;
}

void circle_test()
{
  int i;
  const double pi = 3.14159265;
  const int m = 256;
  const int step = 1;
  SDL_Event event;

  for(i=1; i<360; i+=step)
  {
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = m*(cos(i*2*pi/360)-cos((i-1)*2*pi/360));
    event.motion.yrel = m*(sin(i*2*pi/360)-sin((i-1)*2*pi/360));
    SDL_PushEvent(&event);
    usleep(REFRESH_PERIOD);
  }
}

static void key(int sym, int down)
{
	pthread_t thread;
  pthread_attr_t thread_attr;

	switch (sym)
  {
	  case SDLK_LCTRL: lctrl = down ? 1 : 0; break;
    case SDLK_RCTRL: rctrl = down ? 1 : 0; break;

    case SDLK_p:
    if(down && lctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)circle_test, NULL);
      }
      break;

    case SDLK_ESCAPE: if(down) done = 1; break;
  }

	if(calibration)
	{
	  switch (sym)
	  {
	    case SDLK_KP_MINUS: if(down) { multiplier_x -= MULTIPLIER_STEP; printf("multiplier_x: %e\n", multiplier_x); } break;
	    case SDLK_KP_PLUS:  if(down) { multiplier_x += MULTIPLIER_STEP; printf("multiplier_x: %e\n", multiplier_x); } break;
	    case SDLK_KP9: if(down) { multiplier_y -= MULTIPLIER_STEP; printf("multiplier_y: %e\n", multiplier_y); } break;
	    case SDLK_KP6:  if(down) { multiplier_y += MULTIPLIER_STEP; printf("multiplier_y: %e\n", multiplier_y); } break;
	    case SDLK_KP_DIVIDE:  if(down) { dead_zone_x -= 1; printf("dead_zone_x: %d\n", dead_zone_x); } break;
	    case SDLK_KP_MULTIPLY:  if(down) { dead_zone_x += 1; printf("dead_zone_x: %d\n", dead_zone_x); } break;
	    case SDLK_KP2:  if(down) { dead_zone_y -= 1; printf("dead_zone_y: %d\n", dead_zone_y); } break;
	    case SDLK_KP3:  if(down) { dead_zone_y += 1; printf("dead_zone_y: %d\n", dead_zone_y); } break;
	    case SDLK_KP7:  if(down) { exponent -= EXPONENT_STEP; printf("exponent: %e\n", exponent); } break;
	    case SDLK_KP8:  if(down) { exponent += EXPONENT_STEP; printf("exponent: %e\n", exponent); } break;
	  }
	}

	if(lctrl && rctrl)
  {
    if(calibration)
    {
      calibration = 0;
      printf("calibration mode disabled\n");
    }
    else
    {
      calibration = 1;
      printf("calibration mode enabled\n");
    }
  }

	if(down) macro_lookup(sym);
}

/*
 * to be deleted later
 */
extern s_mouse_control mouse_control[MAX_DEVICES];

int main(int argc, char *argv[])
{
    SDL_Event events[EVENT_BUFFER_SIZE];
    SDL_Event* event;
    SDL_Event mouse_raz;
    int i;
    int num_evt;
    unsigned char buf[48];

    read_config_dir();

    initialize_macros();

    for(i=0; i<MAX_CONTROLLERS; ++i)
    {
      sixaxis_init(state+i);
      memset(controller+i, 0x00, sizeof(s_controller));
    }

    if (!initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Sixaxis Control"))
        err(1, "can't init sdl");

    if(tcpconnect() < 0)
    {
      err(1, "tcpconnect");
    }

    done = 0;
    while(!done)
    {
        SDL_PumpEvents();
        num_evt = SDL_PeepEvents(events, sizeof(events)/sizeof(events[0]), SDL_GETEVENT, SDL_ALLEVENTS);

        for(event=events; event<events+num_evt; ++event)
        {
          process_event(event);

          trigger_lookup(event);

          switch (event->type)
          {
            case SDL_QUIT:
              done = 1;
              break;
            case SDL_KEYDOWN:
              key(event->key.keysym.sym, 1);
              break;
            case SDL_KEYUP:
              key(event->key.keysym.sym, 0);
              break;
          }
        }

        /*
         * This processes an extra event to stop axis movements.
         */
        for(i=0; i<MAX_DEVICES; ++i)
        {
          if(mouse_control[i].changed)
          {
            if(!mouse_control[i].nb_motion)
            {
              mouse_raz.motion.which = i;
              mouse_raz.type = SDL_MOUSEMOTION;
              mouse_raz.motion.xrel = 0;
              mouse_raz.motion.yrel = 0;
              process_event(&mouse_raz);
              mouse_control[i].changed = 0;
            }
          }
          mouse_control[i].merge_x = 0;
          mouse_control[i].merge_y = 0;
          mouse_control[i].nb_motion = 0;
        }

        /*
         * Send a command to each controller that has its status changed.
         */
        for(i=0; i<MAX_CONTROLLERS; ++i)
        {
          if(controller[i].send_command)
          {
            if (assemble_input_01(buf, sizeof(buf), state+i) < 0)
            {
              printf("can't assemble\n");
            }

            send(sockfd[i], buf, 48, MSG_DONTWAIT);

            if (debug)
            {
              sixaxis_dump_state(state+i);
            }

            controller[i].send_command = 0;
          }
        }
        usleep(REFRESH_PERIOD);
    }

    printf("Exiting\n");
    SDL_Quit();

    free_macros();

    return 0;
}
