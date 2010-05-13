/* Sixaxis emulator

   Copyright (c) 2010 Mathieu Laurendeau

   Copyright (c) 2009 Jim Paris <jim@jtan.com>
   License: GPLv3
*/

#include <termio.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <math.h>
#include <SDL/SDL.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "sixaxis.h"
#include "dump.h"
#include <pthread.h>
#include "macros.h"

#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define DEFAULT_DEAD_ZONE 18
#define DEFAULT_MULTIPLIER 4
#define REFRESH_PERIOD 10000 //=10ms
#define EVENT_BUFFER_SIZE 32

static int debug = 0;
static int done = 0;
static double multiplier = DEFAULT_MULTIPLIER;
static int dead_zone = DEFAULT_DEAD_ZONE;
static int dead_zone_calibration = 0;

SDL_Surface *screen = NULL;

int initialize(int width, int height, const char *title)
{
	/* Init SDL */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		return 0;
	}
	
	SDL_WM_SetCaption(title, title);

	/* Init video */
	screen = SDL_SetVideoMode(width, height, 0, SDL_HWSURFACE | SDL_ANYFORMAT);
	if (screen == NULL)
	{
		fprintf(stderr, "Unable to create video surface: %s\n", SDL_GetError());
		return 0;
	}

	return 1;
}

#define TCP_PORT 21313

int tcpconnect(void)
{
	int fd;
	struct sockaddr_in addr;

	if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		err(1, "socket");
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(TCP_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		err(1, "connect");
	return fd;
}

struct sixaxis_state state;
int (*assemble)(uint8_t *buf, int len, struct sixaxis_state *state);
int sockfd;

void move_x(int x)
{
	state.user.axis[0].x = x;
}

void move_y(int y)
{
	state.user.axis[0].y = y;
}

void key(int sym, int down)
{
	int index = -1;
	pthread_t thread;
	pthread_attr_t thread_attr;
	SDLKey key;

	switch (sym) {
	case SDLK_RSHIFT:	index = sb_ps;		break;

	case SDLK_e:		index = sb_triangle;	break;
	case SDLK_SPACE:	index = sb_cross;	break;
	case SDLK_LCTRL:	index = sb_circle;	break;
	case SDLK_r:		index = sb_square;	break;

	case SDLK_TAB: 		index = sb_select;	break;
	case SDLK_BACKSPACE:    index = sb_start;	break;

	case SDLK_t:        	index = sb_l2;		break;
	case SDLK_k:        	index = sb_l1;		break;
	case SDLK_l:        	index = sb_r1;		break;
	case SDLK_g:	        index = sb_r2;		break;
	case SDLK_LSHIFT:       index = sb_l3;		break;
	case SDLK_f:	        index = sb_r3;		break;
	
	case SDLK_AMPERSAND:	index = sb_up;		break;
	case SDLK_WORLD_73:	 	index = sb_down;	break;
	case SDLK_QUOTEDBL:	 	index = sb_left;	break;
	case SDLK_QUOTE: 		index = sb_right;	break;
	
	case SDLK_q:		down?move_x(-127):move_x(0);	break;
	case SDLK_z:	 	down?move_y(-127):move_y(0);	break;
	case SDLK_s:	 	down?move_y(127):move_y(0);	break;
	case SDLK_d: 		down?move_x(127):move_x(0);	break;

	case SDLK_ESCAPE:	if(down) done = 1; break;
	case SDLK_KP_MINUS:	if(down) { multiplier -= 0.5; printf("multiplier: %e\n", multiplier); } break;
	case SDLK_KP_PLUS: 	if(down) { multiplier += 0.5; printf("multiplier: %e\n", multiplier); } break;
	case SDLK_KP_DIVIDE:	if(down && dead_zone_calibration) { dead_zone -= 1; printf("dead_zone: %d\n", dead_zone); } break;
	case SDLK_KP_MULTIPLY: 	if(down && dead_zone_calibration) { dead_zone += 1; printf("dead_zone: %d\n", dead_zone); } break;
	case SDLK_INSERT:
		if(down)
		{
			if(dead_zone_calibration)
			{
				dead_zone_calibration = 0;
				printf("dead zone calibration mode disabled\n");
			}
			else
			{
				dead_zone_calibration = 1;
				printf("dead zone calibration mode enabled\n");
			}
		}
		break;
	case SDLK_F1:
		if(down)
		{
			key = SDLK_F1;
			pthread_attr_init(&thread_attr);
			pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
			pthread_create( &thread, &thread_attr, (void*)macro, (void*) &key);
			break;
		}
	}

	if (index >= 0) {
		state.user.button[index].pressed = down ? 1 : 0;
		state.user.button[index].value = down ? 255 : 0;
	}
}

void move_z_rz(int z, int rz)
{	
	if(z > 0) state.user.axis[1].x = dead_zone + z;
        else if(z < 0) state.user.axis[1].x = z - dead_zone;
	else state.user.axis[1].x = 0;
	if(rz > 0) state.user.axis[1].y = dead_zone + rz;
        else if(rz < 0) state.user.axis[1].y = rz - dead_zone;
	else state.user.axis[1].y = 0;
}

void clic(int button, int down)
{
	int index = -1;
	
	switch(button)
	{
	case SDL_BUTTON_LEFT: 		index = sb_r1; break;
	case SDL_BUTTON_RIGHT: 		index = sb_l1; break;
	case SDL_BUTTON_MIDDLE: 	index = sb_triangle; break;
	case SDL_BUTTON_WHEELUP: 	index = sb_r2; break;
	case SDL_BUTTON_WHEELDOWN: 	index = sb_l2; break;
	case 8: 	                index = sb_r3; break; //Logitech G5 side button
	}

	if (index >= 0) {
		state.user.button[index].pressed = down ? 1 : 0;
		state.user.button[index].value = down ? 255 : 0;
	}
}

int main(int argc, char *argv[])
{
	SDL_Event events[EVENT_BUFFER_SIZE];
	SDL_Event* event;
	int i;
	int cpt = 0;
	int num_evt;
	unsigned char buf[48];
	int merge_x, merge_y;
	int nb_motion;
	int report_wheel_up, report_wheel_down;
	int change;
	int send_command;
	
	initialize_macros();
	
	sixaxis_init(&state);
	for (i = 0; sixaxis_assemble[i].func; i++)
		if (sixaxis_assemble[i].type == HID_TYPE_INPUT &&
		    sixaxis_assemble[i].report == 0x01) {
			assemble = sixaxis_assemble[i].func;
			break;
		}
	if (!sixaxis_assemble[i].func)
		errx(1, "can't find assemble function");
			
	if (!initialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Sixaxis Control"))
		errx(1, "can't init sdl");

	SDL_WM_GrabInput(SDL_GRAB_ON);
	SDL_ShowCursor(SDL_DISABLE);

	sockfd = tcpconnect();

	done = 0;
	while(!done) 
	{
		SDL_PumpEvents();
		num_evt = SDL_PeepEvents(events, sizeof(events)/sizeof(events[0]), SDL_GETEVENT, SDL_ALLEVENTS);
		if(num_evt > 0)
		{
			change = 1;
			send_command = 1;
			merge_x = 0;
			merge_y = 0;
			nb_motion = 0;
			report_wheel_up = 0;
			report_wheel_down = 0;
			//if(num_evt>1) printf("%d\n", num_evt);
			for(event=events; event<events+num_evt; ++event)
			{
				switch( event->type ) {
				case SDL_QUIT:
					done = 1;
					break;
				case SDL_KEYDOWN:
					key(event->key.keysym.sym, 1);
					break;
				case SDL_KEYUP:
					key(event->key.keysym.sym, 0);
					break;
				case SDL_MOUSEMOTION:
					merge_x += multiplier*event->motion.xrel;
					merge_y += multiplier*event->motion.yrel;
					nb_motion++;
					cpt = 0;
					break;
				case SDL_MOUSEBUTTONDOWN:
					clic(event->button.button, 1);
					if(event->button.button == SDL_BUTTON_WHEELUP)
					{
						report_wheel_up = 1;
					}
					else if(event->button.button == SDL_BUTTON_WHEELDOWN)
					{
						report_wheel_down = 1;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					if(report_wheel_up && event->button.button == SDL_BUTTON_WHEELUP)
					{
						SDL_PushEvent(event);
					}
					else if(report_wheel_down && event->button.button == SDL_BUTTON_WHEELDOWN)
					{
						SDL_PushEvent(event);
					}
					else
					{
						clic(event->button.button, 0);
					}
					break;
				}
				
			}
			if(nb_motion) move_z_rz(merge_x/nb_motion, merge_y/nb_motion);
			else move_z_rz(0, 0);
		}
		else
		{
			if(change)
			{
				move_z_rz(0, 0);
				change = 0;
				send_command = 1;
			}
			if(dead_zone_calibration)
			{
				move_z_rz(dead_zone_calibration, 0);
				send_command = 1;
			}
		}
		if(send_command)
		{
			if (assemble(buf, sizeof(buf), &state) < 0)
				printf("can't assemble\n");
			send(sockfd, buf, 48, MSG_DONTWAIT);
			if(debug) sixaxis_dump_state(&state);
			send_command = 0;
		}
		usleep(REFRESH_PERIOD);
	}

	printf("Exiting\n");
	SDL_Quit();
	
	free_macros();
	
	return 0;
}
