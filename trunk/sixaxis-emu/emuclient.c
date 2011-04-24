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
#else
#include <winsock2.h>
#define MSG_DONTWAIT 0
#define sleep Sleep
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
#include <sys/time.h>

#define SCREEN_WIDTH  8
#define SCREEN_HEIGHT 8
#define MULTIPLIER_STEP 0.01
#define EXPONENT_STEP 0.01
#define EVENT_BUFFER_SIZE 256

static const double pi = 3.14159265;

char* homedir;

int done = 0;
int current_mouse = 0;
int current_conf = 0;
e_current_cal current_cal = NONE;
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

extern s_mouse_control mouse_control[MAX_DEVICES];
extern s_mouse_cal mouse_cal[MAX_DEVICES][MAX_CONFIGURATIONS];

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
char* mouseName[MAX_DEVICES] = {};
int mouseVirtualIndex[MAX_DEVICES] = {};
char* keyboardName[MAX_DEVICES] = {};
int keyboardVirtualIndex[MAX_DEVICES] = {};

#define BT_SIXAXIS_NAME "PLAYSTATION(R)3 Controller"

int initialize(int width, int height, const char *title)
{
  int i = 0;
  int j;
  SDL_Joystick* joystick;
#ifndef WIN32
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
#ifndef WIN32
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
  const int step = 1;

  for(i=1; i<360; i+=step)
  {
    mouse_control[current_mouse].merge_x = round(mouse_cal[current_mouse][current_conf].rd*64*(cos(i*2*pi/360)-cos((i-1)*2*pi/360)));
    mouse_control[current_mouse].merge_y = round(mouse_cal[current_mouse][current_conf].rd*64*(sin(i*2*pi/360)-sin((i-1)*2*pi/360)));
    mouse_control[current_mouse].change = 1;
    usleep(REFRESH_PERIOD);
  }
}

void display_calibration()
{
  if(current_cal != NONE)
  {
    printf("calibrating mouse %s %d\n", mouseName[current_mouse], mouseVirtualIndex[current_mouse]);
    printf("calibrating conf %d\n", current_conf+1);
    printf("multiplier_x:");
    if(mouse_cal[current_mouse][current_conf].mx)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].mx);
    }
    else
    {
      printf(" NA\n");
    }
    printf("multiplier_y:");
    if(mouse_cal[current_mouse][current_conf].my)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].my);
    }
    else
    {
      printf(" NA\n");
    }
    printf("dead_zone_x:");
    if(mouse_cal[current_mouse][current_conf].dzx)
    {
      printf(" %d\n", *mouse_cal[current_mouse][current_conf].dzx);
    }
    else
    {
      printf(" NA\n");
    }
    printf("dead_zone_y:");
    if(mouse_cal[current_mouse][current_conf].dzy)
    {
      printf(" %d\n", *mouse_cal[current_mouse][current_conf].dzy);
    }
    else
    {
      printf(" NA\n");
    }
    printf("shape:");
    if(mouse_cal[current_mouse][current_conf].dzs) {
      if(*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE) printf(" Circle\n");
      else printf(" Rectangle\n");
    }
    else
    {
      printf(" NA\n");
    }
    printf("exponent_x:");
    if(mouse_cal[current_mouse][current_conf].ex)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].ex);
    }
    else
    {
      printf(" NA\n");
    }
    printf("exponent_y:");
    if(mouse_cal[current_mouse][current_conf].ey)
    {
      printf(" %.2f\n", *mouse_cal[current_mouse][current_conf].ey);
    }
    else
    {
      printf(" NA\n");
    }
    printf("radius: %.2f\n", mouse_cal[current_mouse][current_conf].rd);
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

    case SDLK_LSHIFT: lshift = down ? 1 : 0; break;
    case SDLK_RSHIFT: rshift = down ? 1 : 0; break;

    case SDLK_LALT: lalt = down ? 1 : 0; break;
    case SDLK_MODE: ralt = down ? 1 : 0; break;

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

  switch (sym)
  {
    case SDLK_F1:
      if(down && rctrl)
      {
        if(current_cal == NONE)
        {
          current_cal = MC;
          printf("mouse selection\n");
          display_calibration();
        }
        else
        {
          current_cal = NONE;
          printf("calibration done\n");
        }
      }
      break;
    case SDLK_F2:
      if(down && rctrl)
      {
        if(current_cal == CC)
        {
          current_cal = MC;
          printf("mouse selection\n");
        }
        else if(current_cal >= MC)
        {
          current_cal = CC;
          printf("config selection\n");
        }
      }
      break;
    case SDLK_F3:
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
        {
          printf("calibrating multiplier x\n");
          current_cal = MX;
        }
      }
      break;
    case SDLK_F4:
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
        {
          printf("calibrating multiplier y\n");
          current_cal = MY;
        }
      }
      break;
    case SDLK_F5:
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
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
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
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
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
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
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
        {
          printf("calibrating exponent x\n");
          current_cal = EX;
        }
      }
      break;
    case SDLK_F9:
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
        {
          printf("calibrating exponent y\n");
          current_cal = EY;
        }
      }
      break;
    case SDLK_F10:
      if(down && rctrl && current_cal != NONE)
      {
        if(current_conf >=0 && current_mouse >=0)
        {
          printf("adjusting circle test radius\n");
          current_cal = RD;
        }
      }
      break;
    case SDLK_F11:
      if(down && rctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)circle_test, NULL);
      }
      break;
    case SDLK_F12:
      if(down && rctrl)
      {
        pthread_attr_init(&thread_attr);
        pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
        pthread_create( &thread, &thread_attr, (void*)auto_test, NULL);
      }
      break;
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

static void button(int which, int button)
{
  switch (button)
  {
    case SDL_BUTTON_WHEELUP:
      switch(current_cal)
      {
        case MC:
          current_mouse += 1;
          if(!mouseName[current_mouse])
          {
            current_mouse -= 1;
          }
          break;
        case CC:
          current_conf += 1;
          if(current_conf > MAX_CONFIGURATIONS-1)
          {
            current_conf = MAX_CONFIGURATIONS-1;
          }
          break;
        case MX:
          if(mouse_cal[current_mouse][current_conf].mx) *mouse_cal[current_mouse][current_conf].mx += MULTIPLIER_STEP;
          break;
        case MY:
          if(mouse_cal[current_mouse][current_conf].my) *mouse_cal[current_mouse][current_conf].my += MULTIPLIER_STEP;
          break;
        case DZX:
          if(mouse_cal[current_mouse][current_conf].dzx)
          {
            *mouse_cal[current_mouse][current_conf].dzx += 1;
            if(*mouse_cal[current_mouse][current_conf].dzx > 127)
            {
              *mouse_cal[current_mouse][current_conf].dzx = 127;
            }
            mouse_control[current_mouse].merge_x = 1;
            mouse_control[current_mouse].merge_y = 0;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZY:
          if(mouse_cal[current_mouse][current_conf].dzy)
          {
            *mouse_cal[current_mouse][current_conf].dzy += 1;
            if(*mouse_cal[current_mouse][current_conf].dzy > 127)
            {
              *mouse_cal[current_mouse][current_conf].dzy = 127;
            }
            mouse_control[current_mouse].merge_x = 0;
            mouse_control[current_mouse].merge_y = 1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZS:
          if(mouse_cal[current_mouse][current_conf].dzs)
          {
            if(*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE)
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
          if(mouse_cal[current_mouse][current_conf].ex) *mouse_cal[current_mouse][current_conf].ex += EXPONENT_STEP;
          break;
        case EY:
          if(mouse_cal[current_mouse][current_conf].ey) *mouse_cal[current_mouse][current_conf].ey += EXPONENT_STEP;
          break;
        case NONE:
          break;
      }
      break;
    case SDL_BUTTON_WHEELDOWN:
      switch(current_cal)
      {
        case MC:
          if(current_mouse > 0)
          {
            current_mouse -= 1;
          }
          break;
        case CC:
          if(current_conf > 0)
          {
            current_conf -= 1;
          }
          break;
        case MX:
          if(mouse_cal[current_mouse][current_conf].mx) *mouse_cal[current_mouse][current_conf].mx -= MULTIPLIER_STEP;
          break;
        case MY:
          if(mouse_cal[current_mouse][current_conf].my) *mouse_cal[current_mouse][current_conf].my -= MULTIPLIER_STEP;
          break;
        case DZX:
          if(mouse_cal[current_mouse][current_conf].dzx)
          {
            *mouse_cal[current_mouse][current_conf].dzx -= 1;
            if(*mouse_cal[current_mouse][current_conf].dzx < 0)
            {
              *mouse_cal[current_mouse][current_conf].dzx = 0;
            }
            mouse_control[current_mouse].merge_x = -1;
            mouse_control[current_mouse].merge_y = 0;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZY:
          if(mouse_cal[current_mouse][current_conf].dzy)
          {
            *mouse_cal[current_mouse][current_conf].dzy -= 1;
            if(*mouse_cal[current_mouse][current_conf].dzy < 0)
            {
              *mouse_cal[current_mouse][current_conf].dzy = 0;
            }
            mouse_control[current_mouse].merge_x = 0;
            mouse_control[current_mouse].merge_y = -1;
            mouse_control[current_mouse].change = 1;
          }
          break;
        case DZS:
          if(mouse_cal[current_mouse][current_conf].dzs)
          {
            if(*mouse_cal[current_mouse][current_conf].dzs == E_SHAPE_CIRCLE)
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
          if(mouse_cal[current_mouse][current_conf].ex) *mouse_cal[current_mouse][current_conf].ex -= EXPONENT_STEP;
          break;
        case EY:
          if(mouse_cal[current_mouse][current_conf].ey) *mouse_cal[current_mouse][current_conf].ey -= EXPONENT_STEP;
          break;
        case NONE:
          break;
      }
      break;
  }

  display_calibration();
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
    struct timeval t0, t1;
    int time_to_sleep;

#ifndef WIN32
    setlinebuf(stdout);
    homedir = getpwuid(getuid())->pw_dir;
#else

#endif

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

        if(num_evt == EVENT_BUFFER_SIZE)
        {
          printf("buffer too small!!!\n");
        }

        for(event=events; event<events+num_evt; ++event)
        {
          if(event->type != SDL_MOUSEMOTION)
          {
            //if calibration is on, all mouse wheel events are skipped
            if(current_cal == NONE || event->type != SDL_MOUSEBUTTONDOWN || (event->button.button != SDL_BUTTON_WHEELDOWN && event->button.button != SDL_BUTTON_WHEELUP))
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
            case SDL_MOUSEBUTTONDOWN:
              button(event->button.which, event->button.button);
              break;
          }
        }

        for(i=0; i<MAX_DEVICES; ++i)
        {
          if(mouse_control[i].changed || mouse_control[i].change)
          {
            mouse_evt.motion.which = i;
            mouse_evt.type = SDL_MOUSEMOTION;
            process_event(&mouse_evt);
          }
          mouse_control[i].merge_x = 0;
          mouse_control[i].merge_y = 0;
          mouse_control[i].changed = mouse_control[i].change;
          mouse_control[i].change = 0;
          if(i == current_mouse && (current_cal == DZX || current_cal == DZY || current_cal == DZS))
          {
            mouse_control[i].changed = 0;
          }
        }

        /*
         * Send a command to each controller that has its status changed.
         */
        for(i=0; i<MAX_CONTROLLERS; ++i)
        {
          if(controller[i].send_command)
          {
            if(!sockfd[i]) continue;

            if (assemble_input_01(buf, sizeof(buf), state+i) < 0)
            {
              printf("can't assemble\n");
            }

#ifndef WIN32
            send(sockfd[i], buf, 48, MSG_DONTWAIT);
#else
            send(sockfd[i], (const char*)buf, 48, MSG_DONTWAIT);
#endif

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
    for(i=0; i<MAX_DEVICES && mouseName[i]; ++i)
    {
      free(mouseName[i]);
    }
    for(i=0; i<MAX_DEVICES && keyboardName[i]; ++i)
    {
      free(keyboardName[i]);
    }
    return 0;
}
