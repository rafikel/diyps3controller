/*
 * sdl_tools.h
 *
 *  Created on: 28 mai 2011
 *      Author: Matlo
 */

#ifndef SDL_TOOLS_H_
#define SDL_TOOLS_H_

#include <SDL/SDL.h>

int sdl_initialize();
void sdl_grab_toggle();
void sdl_grab();
void sdl_release_unused();
void sdl_quit();
void sdl_free_mk();

#endif /* SDL_TOOLS_H_ */
