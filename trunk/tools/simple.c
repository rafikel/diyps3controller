/* 
 * Original code from http://www.u-picardie.fr/ferment/xwindow/pointer.htm
 * Compile with: gcc -lX11 -o simple simple.c
 */

/* pointer.c */

/* mouvement de souris */

#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <sys/time.h>

GC      gc; 
Display *display; 
int     screen; 
Window  win, root, wbidon; 
unsigned long white_pixel, black_pixel;
XSetWindowAttributes xattrib;
unsigned int ubidon;
int x, y, bidon, i = 0;

void print_time()
{
  struct timeval tv;

  gettimeofday(&tv, NULL);
  printf("time: %d.%06d\n", (int)tv.tv_sec, (int)tv.tv_usec);
}

main() { 
  int time;
  if ((display = XOpenDisplay ("")) == NULL) { 
    fprintf (stderr, "Can't open Display\n"); 
    exit (1); 
  } 
  gc = DefaultGC (display, screen); 
  screen = DefaultScreen (display); 
  root = RootWindow (display, screen); 
  white_pixel = WhitePixel (display, screen); 
  black_pixel = BlackPixel (display, screen); 
  win = XCreateSimpleWindow (display, root,  
			     0, 0, 200, 100, 5, black_pixel, white_pixel); 
  XSelectInput (display, win, PointerMotionMask | ButtonPressMask |
		ExposureMask); 
  XStoreName (display, win, "pointer"); 
  XMapWindow (display, win); 
 
  for (;;) { 
    XEvent ev;
    XNextEvent (display, &ev);
    switch (ev.type) {
    case MotionNotify :
      printf("motion: ");
      print_time();
      break ;
    case ButtonPress  :
      printf("button press: ");
      print_time();
    default :
      break;
    } 
  } 
}

