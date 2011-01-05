
#ifndef MACROS_H
#define MACROS_H

#include <SDL/SDL.h>

void macro_lookup(SDLKey);
void initialize_macros();
void free_macros();

SDLKey get_key_from_buffer(const char*);

#endif
