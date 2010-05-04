#include <curses.h>
#include <form.h>
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
#include <arpa/inet.h>
#include <sys/socket.h>
#include "sixaxis.h"
#include "dump.h"

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

void key(int sym, int down)
{
	unsigned char buf[48];
	int index = -1;

	switch (sym) {
	case 'p':	 index = sb_ps;		break;

	case 'w':	 index = sb_triangle;	break;
	case 'x':	 index = sb_cross;	break;
	case 'd':	 index = sb_circle;	break;
	case 'a':	 index = sb_square;	break;

	case 'g':        index = sb_select;	break;
	case 'h':        index = sb_start;	break;

	case '1':        index = sb_l2;		break;
	case '2':        index = sb_l1;		break;
	case '3':        index = sb_r1;		break;
	case '4':        index = sb_r2;		break;

	case SDLK_UP:	 index = sb_up;		break;
	case SDLK_DOWN:	 index = sb_down;	break;
	case SDLK_LEFT:	 index = sb_left;	break;
	case SDLK_RIGHT: index = sb_right;	break;
	}

	if (index >= 0) {
		state.user.button[index].pressed = down ? 1 : 0;
		state.user.button[index].value = down ? 255 : 0;
	}

	if (assemble(buf, sizeof(buf), &state) < 0)
		printf("can't assemble\n");

	send(sockfd, buf, 48, MSG_DONTWAIT);
	sixaxis_dump_state(&state);
}

static void finish(int sig)
{
	endwin();
	exit(0);
}

int main(int argc, char *argv[])
{
	int done;
	int i;
	
	initscr();
	keypad(stdscr, TRUE);
	nonl();
	cbreak();
	noecho();


	sixaxis_init(&state);
	for (i = 0; sixaxis_assemble[i].func; i++)
		if (sixaxis_assemble[i].type == HID_TYPE_INPUT &&
		    sixaxis_assemble[i].report == 0x01) {
			assemble = sixaxis_assemble[i].func;
			break;
		}
	if (!sixaxis_assemble[i].func)
		errx(1, "can't find assemble function");
			
	if (!initialize(320, 240, "Sixaxis Control"))
		errx(1, "can't init sdl");

	sockfd = tcpconnect();

	done = 0;
	while(!done) 
	{
		if(SDL_WaitEvent(&event)) 
		{
			switch( event.type ) {
			case SDL_QUIT:
				done = 1;
				break;
			case SDL_KEYDOWN:
				if(event.key.keysym.sym == SDLK_ESCAPE)
					done = 1;
				else 
					key(event.key.keysym.sym, 1);
				break;
			case SDL_KEYUP:
				key(event.key.keysym.sym, 0);
				break;
			}
		}
	}

	printf("Exiting\n");
	SDL_Quit();
	
	return 0;
}
