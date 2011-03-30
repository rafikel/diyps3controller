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
#include <pwd.h>

#include <pthread.h>
#include <sys/time.h>

#define SCREEN_WIDTH  8
#define SCREEN_HEIGHT 8
#define DEFAULT_DEAD_ZONE_X 18
#define DEFAULT_DEAD_ZONE_Y 20
#define DEFAULT_MULTIPLIER_X 4
#define DEFAULT_MULTIPLIER_Y 9
#define DEFAULT_EXPONENT 1
#define MULTIPLIER_STEP 0.25
#define EXPONENT_STEP 0.01
#define REFRESH_PERIOD 10000 //=10ms
#define EVENT_BUFFER_SIZE 32

char* homedir;

int done = 0;
double multiplier_x = DEFAULT_MULTIPLIER_X;
double multiplier_y = DEFAULT_MULTIPLIER_Y;
double exponent = DEFAULT_EXPONENT;
int dead_zone_x = DEFAULT_DEAD_ZONE_X;
int dead_zone_y = DEFAULT_DEAD_ZONE_Y;
int calibration = 0;
int testing = 0;
e_shape test_shape;
int display = 0;
static int lctrl = 0;
static int rctrl = 0;
static int lshift = 0;
static int rshift = 0;
static int lalt = 0;
static int ralt = 0;
static int grab = 1;

SDL_Surface *screen = NULL;

struct sixaxis_state state[MAX_CONTROLLERS];
int (*assemble)(uint8_t *buf, int len, struct sixaxis_state *state);
static int sockfd[MAX_CONTROLLERS];
s_controller controller[MAX_CONTROLLERS] = {};

/*
 * to be deleted later
 */
extern s_mouse_control mouse_control[MAX_DEVICES];

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


char* joystickName[MAX_DEVICES] = {};
int joystickVirtualIndex[MAX_DEVICES] = {};
int joystickNbButton[MAX_DEVICES] = {};
int joystickSixaxis[MAX_DEVICES] = {};
#ifdef MULTIPLE_MICE_KB
char* mouseName[MAX_DEVICES] = {};
int mouseVirtualIndex[MAX_DEVICES] = {};
char* keyboardName[MAX_DEVICES] = {};
int keyboardVirtualIndex[MAX_DEVICES] = {};
#endif

#define BT_SIXAXIS_NAME "PLAYSTATION(R)3 Controller"

int initialize(int width, int height, const char *title)
{
  int i = 0;
  int j;
  SDL_Joystick* joystick;
#ifdef MULTIPLE_MICE_KB
  const char* name;
#endif

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

  if(grab)
  {
    SDL_WM_GrabInput(SDL_GRAB_ON);
  }
  SDL_ShowCursor(SDL_DISABLE);

  while((joystick = SDL_JoystickOpen(i)))
  {
      joystickName[i] = strdup(SDL_JoystickName(i));

      if(!strncmp(joystickName[i], BT_SIXAXIS_NAME, sizeof(BT_SIXAXIS_NAME)-1))
      {
        joystickName[i][sizeof(BT_SIXAXIS_NAME)-1] = '\0';
      }

      for(j=i-1; j>=0; --j)
      {
        if(!strcmp(joystickName[i], joystickName[j]))
        {
          joystickVirtualIndex[i] = joystickVirtualIndex[j]+1;
          break;
        }
      }
      if(j < 0)
      {
        joystickVirtualIndex[i] = 0;
      }
      joystickNbButton[i] = SDL_JoystickNumButtons(joystick);
      if(!strcmp(joystickName[i], "Sony PLAYSTATION(R)3 Controller"))
      {
        joystickSixaxis[i] = 1;
      }
      i++;
  }
#ifdef MULTIPLE_MICE_KB
  i = 0;
  while ((name = SDL_GetMouseName(i)))
  {
    mouseName[i] = strdup(name);

    for (j = i - 1; j >= 0; --j)
    {
      if (!strcmp(mouseName[i], mouseName[j]))
      {
        mouseVirtualIndex[i] = mouseVirtualIndex[j] + 1;
        break;
      }
    }
    if (j < 0)
    {
      mouseVirtualIndex[i] = 0;
    }
    i++;
  }
  i = 0;
  while ((name = SDL_GetKeyboardName(i)))
  {
    keyboardName[i] = strdup(name);

    for (j = i - 1; j >= 0; --j)
    {
      if (!strcmp(keyboardName[i], keyboardName[j]))
      {
        keyboardVirtualIndex[i] = keyboardVirtualIndex[j] + 1;
        break;
      }
    }
    if (j < 0)
    {
      keyboardVirtualIndex[i] = 0;
    }
    i++;
  }
#endif
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
      }
      else
      {
        ret = 1;
        printf("connected to emu %d\n", i);
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

void auto_test()
{

  state[0].user.axis[1][0] = 48;
  controller[0].send_command = 1;

  sleep(18);

  state[0].user.axis[1][0] = 0;
  controller[0].send_command = 1;

  sleep(2);

  state[0].user.axis[1][0] = 88;
  controller[0].send_command = 1;

  sleep(5);

  state[0].user.axis[1][0] = 0;
  controller[0].send_command = 1;

  sleep(2);

  state[0].user.axis[1][0] = 128;
  controller[0].send_command = 1;

  sleep(2);

  state[0].user.axis[1][0] = 0;
  controller[0].send_command = 1;

  sleep(2);
}

void circle_test()
{
  int i;
#ifdef MULTIPLE_MICE_KB
  int j;
#endif
  const double pi = 3.14159265;
  const int m = 294;
  const int step = 1;
  SDL_Event event = {};

  for(i=1; i<360; i+=step)
  {
#ifdef MULTIPLE_MICE_KB
    for(j=0; j<MAX_DEVICES && mouseName[j]; ++j)
    {
      event.motion.which = j;
      event.type = SDL_MOUSEMOTION;
      event.motion.xrel = round(m*(cos(i*2*pi/360)-cos((i-1)*2*pi/360)));
      event.motion.yrel = round(m*(sin(i*2*pi/360)-sin((i-1)*2*pi/360)));
      SDL_PushEvent(&event);
    }
#else
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = round(m*(cos(i*2*pi/360)-cos((i-1)*2*pi/360)));
    event.motion.yrel = round(m*(sin(i*2*pi/360)-sin((i-1)*2*pi/360)));
    SDL_PushEvent(&event);
#endif
    usleep(REFRESH_PERIOD);
  }
}

void dz_test()
{
  int i;
#ifdef MULTIPLE_MICE_KB
  int j;
#endif
  SDL_Event event = {};

  testing = 1;

#ifdef MULTIPLE_MICE_KB
  for(i=0; i<200; ++i)
  {
    for(j=0; j<MAX_DEVICES && mouseName[j]; ++j)
    {
      event.motion.which = j;
      event.type = SDL_MOUSEMOTION;
      event.motion.xrel = 1;
      event.motion.yrel = 0;
      SDL_PushEvent(&event);
    }
    usleep(REFRESH_PERIOD);
  }

  for(i=0; i<200; ++i)
  {
    for(j=0; j<MAX_DEVICES && mouseName[j]; ++j)
    {
      event.motion.which = j;
      event.type = SDL_MOUSEMOTION;
      event.motion.xrel = 1;
      event.motion.yrel = 1;
      SDL_PushEvent(&event);
    }
    usleep(REFRESH_PERIOD);
  }

  for(i=0; i<200; ++i)
  {
    for(j=0; j<MAX_DEVICES && mouseName[j]; ++j)
    {
      event.motion.which = j;
      event.type = SDL_MOUSEMOTION;
      event.motion.xrel = 0;
      event.motion.yrel = 1;
      SDL_PushEvent(&event);
    }
    usleep(REFRESH_PERIOD);
  }
#else
  for(i=0; i<200; ++i)
  {
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 1;
    event.motion.yrel = 0;
    SDL_PushEvent(&event);
    usleep(REFRESH_PERIOD);
  }

  for(i=0; i<200; ++i)
  {
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 1;
    event.motion.yrel = 1;
    SDL_PushEvent(&event);
    usleep(REFRESH_PERIOD);
  }

  for(i=0; i<200; ++i)
  {
    event.type = SDL_MOUSEMOTION;
    event.motion.xrel = 0;
    event.motion.yrel = 1;
    SDL_PushEvent(&event);
    usleep(REFRESH_PERIOD);
  }
#endif

  sleep(1);//give time to purge events

  testing = 0;
}

void display_calibration()
{
  printf("multiplier_x: %.2f\n", multiplier_x);
  printf("multiplier_y: %.2f\n", multiplier_y);
  printf("dead_zone_x: %d\n", dead_zone_x);
  printf("dead_zone_y: %d\n", dead_zone_y);
  printf("exponent: %.2f\n", exponent);
  if(test_shape == E_SHAPE_CIRCLE) printf("shape: Circle\n");
  else printf("shape: Rectangle\n");
}

static void key(int sym, int down)
{
  pthread_t thread;
  pthread_attr_t thread_attr;
  int i;

	switch (sym)
  {
	  case SDLK_LCTRL: lctrl = down ? 1 : 0; break;
    case SDLK_RCTRL: rctrl = down ? 1 : 0; break;

    case SDLK_LSHIFT: lshift = down ? 1 : 0; break;
    case SDLK_RSHIFT: rshift = down ? 1 : 0; break;

    case SDLK_LALT: lalt = down ? 1 : 0; break;
    case SDLK_MODE: ralt = down ? 1 : 0; break;

    case SDLK_p:
    if(down && rctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)circle_test, NULL);
      }
      break;

    case SDLK_o:
    if(down && rctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)auto_test, NULL);
      }
      break;

    case SDLK_ESCAPE: if(down) done = 1; break;
  }

	if(lalt && ralt)
	{
	  if(grab)
	  {
	    SDL_WM_GrabInput(SDL_GRAB_OFF);
	    grab = 0;
	  }
	  else
	  {
	    SDL_WM_GrabInput(SDL_GRAB_ON);
      grab = 1;
	  }
	}

	if(calibration)
	{
	  switch (sym)
	  {
	    case SDLK_KP2: if(down) { multiplier_x -= MULTIPLIER_STEP; printf("multiplier_x: %.2f\n", multiplier_x); } break;
	    case SDLK_KP5:  if(down) { multiplier_x += MULTIPLIER_STEP; printf("multiplier_x: %.2f\n", multiplier_x); } break;
	    case SDLK_KP8: if(down) { multiplier_y -= MULTIPLIER_STEP; printf("multiplier_y: %.2f\n", multiplier_y); } break;
	    case SDLK_KP_DIVIDE:  if(down) { multiplier_y += MULTIPLIER_STEP; printf("multiplier_y: %.2f\n", multiplier_y); } break;
	    case SDLK_KP3:  if(down) { dead_zone_x -= 1; printf("dead_zone_x: %d\n", dead_zone_x); } break;
	    case SDLK_KP6:  if(down) { dead_zone_x += 1; printf("dead_zone_x: %d\n", dead_zone_x); } break;
	    case SDLK_KP9:  if(down) { dead_zone_y -= 1; printf("dead_zone_y: %d\n", dead_zone_y); } break;
	    case SDLK_KP_MULTIPLY:  if(down) { dead_zone_y += 1; printf("dead_zone_y: %d\n", dead_zone_y); } break;
	    case SDLK_KP1:  if(down) { exponent -= EXPONENT_STEP; printf("exponent: %.2f\n", exponent); } break;
	    case SDLK_KP4:  if(down) { exponent += EXPONENT_STEP; printf("exponent: %.2f\n", exponent); } break;
	    case SDLK_KP0:
	      if(down)
	      {
	        pthread_attr_init(&thread_attr);
            pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
            pthread_create( &thread, &thread_attr, (void*)dz_test, NULL);
	      }
	      break;

	    case SDLK_KP_PERIOD:
	      if(down)
	      {
	        if(test_shape == E_SHAPE_CIRCLE)
	        {
	          test_shape = E_SHAPE_RECTANGLE;
	          printf("shape: Rectangle\n");
	        }
	        else
	        {
	          test_shape = E_SHAPE_CIRCLE;
	          printf("shape: Circle\n");
	        }
	      }
	      break;
	  }
	  for(i=0; i<MAX_DEVICES; ++i)
    {
      mouse_control[i].changed = 1;
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
      display_calibration();
    }
  }

	if(lshift && rshift)
  {
    if(display)
    {
      display = 0;
    }
    else
    {
      display = 1;
    }
  }

	if(down) macro_lookup(sym);
}

int main(int argc, char *argv[])
{
    SDL_Event events[EVENT_BUFFER_SIZE];
    SDL_Event* event;
    SDL_Event mouse_evt = {};
    int i;
    int num_evt;
    unsigned char buf[48];
    int read = 0;
    char* file = NULL;
    double ratio;
    struct timeval t0, t1;
    int time_to_sleep;
    
    setlinebuf(stdout);

    homedir = getpwuid(getuid())->pw_dir;

    system("test -d ~/.emuclient || cp -r /etc/emuclient ~/.emuclient");
    
    for(i=1; i<argc; ++i)
    {
      if(!strcmp(argv[i], "--nograb"))
      {
        grab = 0;
      }
      else if(!strcmp(argv[i], "--config") && i<argc)
      {
        file = argv[++i];
        read = 1;
      }
      else if(!strcmp(argv[i], "--status"))
      {
        display = 1;
      }
      else if(!strcmp(argv[i], "--calibrate"))
      {
        calibration = 1;
        display_calibration();
      }
    }

    if(grab == 1)
    {
      sleep(1);//ugly stuff that needs to be cleaned...
    }

    initialize_macros();

    for(i=0; i<MAX_CONTROLLERS; ++i)
    {
      sixaxis_init(state+i);
      memset(controller+i, 0x00, sizeof(s_controller));
    }

    if (!initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Sixaxis Control"))
        err(1, "can't init sdl");

    if(read == 1)
    {
      read_config_file(file);
    }

    if(tcpconnect() < 0)
    {
      err(1, "tcpconnect");
    }

    done = 0;
    while(!done)
    {
        gettimeofday(&t0, NULL);

        SDL_PumpEvents();
        num_evt = SDL_PeepEvents(events, sizeof(events)/sizeof(events[0]), SDL_GETEVENT, SDL_ALLEVENTS);

        if(num_evt > EVENT_BUFFER_SIZE)
        {
          printf("buffer too small!!!\n");
        }

        for(event=events; event<events+num_evt; ++event)
        {
          if(event->type != SDL_MOUSEMOTION)
          {
            /*
             * Don't process events except mouse events if calibration is on.
             */
            if(!calibration)
            {
              process_event(event);
            }
          }
          else
          {
            ratio = (double)(event->motion.timestamp - mouse_control[event->motion.which].last_timestamp)/10;
            if(ratio <= 0) ratio = 1;//mouse calibration
            if(ratio > 1) ratio = 0.5;//average
            mouse_control[event->motion.which].merge_x += (event->motion.xrel*ratio);
            /*printf("x: %d\n", event->motion.xrel);
            printf("ratio: %.4f\n", ratio);
            printf("add: %.4f\n", event->motion.xrel*ratio);
            printf("merge_x: %.4f\n", mouse_control[event->motion.which].merge_x);*/
            mouse_control[event->motion.which].merge_y += (event->motion.yrel*ratio);
            mouse_control[event->motion.which].nb_motion = 1;
            mouse_control[event->motion.which].changed = 1;
            mouse_control[event->motion.which].last_timestamp = event->motion.timestamp;
          }

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

        for(i=0; i<MAX_DEVICES; ++i)
        {
          if(mouse_control[i].changed)
          {
            mouse_evt.motion.which = i;
            mouse_evt.type = SDL_MOUSEMOTION;
            if(!mouse_control[i].nb_motion)
            {
              mouse_control[i].changed = 0;
            }
            process_event(&mouse_evt);
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

            if (display)
            {
              sixaxis_dump_state(state+i, i);
            }

            controller[i].send_command = 0;
          }
        }

        gettimeofday(&t1, NULL);

        time_to_sleep = REFRESH_PERIOD - ((t1.tv_sec-t0.tv_sec)*1000000+t1.tv_usec-t0.tv_usec);

        if(time_to_sleep > 0)
        {
          usleep(time_to_sleep);
        }
        else
        {
          printf("processing time higher than 10ms!!\n");
        }
    }

    printf("Exiting\n");
    SDL_Quit();

    free_macros();

    for(i=0; i<MAX_DEVICES && joystickName[i]; ++i)
    {
      free(joystickName[i]);
    }
#ifdef MULTIPLE_MICE_KB
    for(i=0; i<MAX_DEVICES && mouseName[i]; ++i)
    {
      free(mouseName[i]);
    }
    for(i=0; i<MAX_DEVICES && keyboardName[i]; ++i)
    {
      free(keyboardName[i]);
    }
#endif
    return 0;
}
