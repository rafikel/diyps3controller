/*
 * sdl_tools.c
 *
 *  Created on: 28 mai 2011
 *      Author: Matlo
 *
 *  License: GPLv3
 */
#include "sdl_tools.h"
#include "config.h"

#ifndef WIN32
#define SCREEN_WIDTH  8
#define SCREEN_HEIGHT 8
#else
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 8
#endif
#define TITLE "Sixaxis Control"
#define BT_SIXAXIS_NAME "PLAYSTATION(R)3 Controller"

char* joystickName[MAX_DEVICES] = {};
SDL_Joystick* joysticks[MAX_DEVICES] = {};
int joystickVirtualIndex[MAX_DEVICES] = {};
int joystickNbButton[MAX_DEVICES] = {};
int joystickSixaxis[MAX_DEVICES] = {};
char* mouseName[MAX_DEVICES] = {};
int mouseVirtualIndex[MAX_DEVICES] = {};
char* keyboardName[MAX_DEVICES] = {};
int keyboardVirtualIndex[MAX_DEVICES] = {};

//needed to release unused joysticks
extern s_mapper* joystick_buttons[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];
extern s_mapper* joystick_axis[MAX_DEVICES][MAX_CONTROLLERS][MAX_CONFIGURATIONS];

static SDL_Surface *screen = NULL;
static int grab = 0;

/*
 * Initializes the SDL library.
 */
int sdl_initialize()
{
  int i = 0;
  int j;
#ifndef WIN32
  const char* name;
#endif

  /* Init SDL */
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
  {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    return 0;
  }

  SDL_WM_SetCaption(TITLE, TITLE);

  /* Init video */
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
      SDL_HWSURFACE | SDL_ANYFORMAT);
  if (screen == NULL)
  {
    fprintf(stderr, "Unable to create video surface: %s\n", SDL_GetError());
    return 0;
  }

  SDL_ShowCursor(SDL_DISABLE);

  while ((joysticks[i] = SDL_JoystickOpen(i)))
  {
    joystickName[i] = strdup(SDL_JoystickName(i));

    if (!strncmp(joystickName[i], BT_SIXAXIS_NAME, sizeof(BT_SIXAXIS_NAME) - 1))
    {
      joystickName[i][sizeof(BT_SIXAXIS_NAME) - 1] = '\0';
    }

    for (j = i - 1; j >= 0; --j)
    {
      if (!strcmp(joystickName[i], joystickName[j]))
      {
        joystickVirtualIndex[i] = joystickVirtualIndex[j] + 1;
        break;
      }
    }
    if (j < 0)
    {
      joystickVirtualIndex[i] = 0;
    }
    joystickNbButton[i] = SDL_JoystickNumButtons(joysticks[i]);
    if (!strcmp(joystickName[i], "Sony PLAYSTATION(R)3 Controller"))
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

/*
 * Release unused stuff.
 * It currently releases unused joysticks, and closes the joystick subsystem if none is used.
 */
void sdl_release_unused()
{
  int i, j, k;
  int used;
  int none = 1;
  for(i=0; i<MAX_DEVICES && joystickName[i]; ++i)
  {
    used = 0;
    for(j=0; j<MAX_CONTROLLERS && !used; ++j)
    {
      for(k=0; k<MAX_CONFIGURATIONS && !used; ++k)
      {
        if((joystick_buttons[i][j][k] && joystick_buttons[i][j][k]->nb_mappers)
            || (joystick_axis[i][j][k] && joystick_axis[i][j][k]->nb_mappers))
        {
          used = 1;
        }
      }
    }
    if(!used)
    {
      printf("close unused joystick: %s\n", joystickName[i]);
      free(joystickName[i]);
      joystickName[i] = NULL;
      SDL_JoystickClose(joysticks[i]);
    }
    else
    {
      none = 0;
    }
  }
  if(none)
  {
    printf("close joystick subsystem\n");
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
  }
}

/*
 * Grab/Release the mouse.
 */
void sdl_grab_toggle()
{
  if (grab)
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

/*
 * Grab the mouse.
 */
void sdl_grab()
{
  SDL_WM_GrabInput(SDL_GRAB_ON);
  grab = 1;
}

void sdl_free_mk()
{
  int i;
  for (i = 0; i < MAX_DEVICES && mouseName[i]; ++i)
  {
    free(mouseName[i]);
    mouseName[i] = NULL;
  }
  for (i = 0; i < MAX_DEVICES && keyboardName[i]; ++i)
  {
    free(keyboardName[i]);
    keyboardName[i] = NULL;
  }
}

/*
 * Free allocated data.
 */
void sdl_quit()
{
  int i;

  for (i = 0; i < MAX_DEVICES; ++i)
  {
    if(joystickName[i])
    {
      free(joystickName[i]);
      SDL_JoystickClose(joysticks[i]);
    }
  }
  for (i = 0; i < MAX_DEVICES && mouseName[i]; ++i)
  {
    free(mouseName[i]);
  }
  for (i = 0; i < MAX_DEVICES && keyboardName[i]; ++i)
  {
    free(keyboardName[i]);
  }
#ifndef WIN32
  SDL_FreeSurface(screen);
  SDL_Quit();
#endif
}
