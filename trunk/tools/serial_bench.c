/*
 Copyright (c) 2011 Mathieu Laurendeau <mat.lau@laposte.net>
 License: GPLv3
 */

//compile with: gcc -g -o serial_bench serial_bench.c -lm

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>
#include <termios.h>
#include <signal.h> //to catch SIGINT
#include <errno.h>
#include <math.h>

static unsigned int baudrate = 0;
static unsigned int rate;

static unsigned int samples = 0;


/*
 * Connect to a serial port.
 */
int serial_connect(char* portname)
{
  struct termios options;

  int fd = open(portname, O_RDWR | O_NOCTTY | O_NDELAY);

  if (fd < 0)
  {
    printf("open error: %s\n", strerror(errno));
    return -1;
  }

  tcgetattr(fd, &options);
  cfsetispeed(&options, baudrate);
  cfsetospeed(&options, baudrate);
  cfmakeraw(&options);
  if (tcsetattr(fd, TCSANOW, &options) < 0)
  {
    printf("can't set serial port options\n");
    close(fd);
    return -1;
  }

  return fd;
}

static int done = 0;

void terminate(int sig)
{
  done = 1;
}

void set_prio()
{
  /*
   * Set realtime priority.
   */
  struct sched_param p =
  { .sched_priority = 99 };
  sched_setscheduler(0, SCHED_FIFO, &p);
}

unsigned int packet_size = 0;

unsigned int* twrite = NULL;
unsigned int* tread = NULL;
unsigned int* ttotal = NULL;

void results(unsigned int* tdiff, unsigned int cpt)
{
  int i;
  int val;
  int sum = 0;
  int average;
  int temp = 0;
  int nbval;
  int worst = 0;

  for (i = 0; i<cpt && tdiff[i]>0; i++)
  {
    sum = sum+tdiff[i];
    if(tdiff[i] > worst)
    {
      worst = tdiff[i];
    }
  }

  nbval = i;

  printf("%d\t", worst);

  if(nbval < 2)
  {
    return;
  }

  average = sum / nbval;

  printf("%d\t", average);

  for (i = 0; i < nbval; i++)
  {
    temp = temp + pow(abs(tdiff[i]-average), 2);
  }

  temp = pow(temp/(nbval-1), 0.5);

  printf("%d\t", temp);
}


static void usage()
{
  fprintf(stderr, "Usage: ./serial_bench [-b baudrate 500000|1000000|1500000|2000000|4000000] [-n samples] [-s packet size]\n");
  exit(EXIT_FAILURE);
}

/*
 * Reads command-line arguments.
 */
static int read_args(int argc, char* argv[])
{
  int flags, opt;
  int nsecs, tfnd;

  nsecs = 0;
  tfnd = 0;
  flags = 0;
  while ((opt = getopt(argc, argv, "b:n:s:")) != -1)
  {
    switch (opt)
    {
      case 'b':
        rate = atoi(optarg);
        switch(rate)
        {
          case 500000:
            baudrate = B500000;
            break;
          case 1000000:
            baudrate = B1000000;
            break;
          case 1500000:
            baudrate = B1500000;
            break;
          case 2000000:
            baudrate = B2000000;
            break;
          case 3000000:
            baudrate = B3000000;
            break;
          case 4000000:
            baudrate = B4000000;
            break;
          default:
            usage();
            break;
        }
        break;
      case 'n':
        samples = atoi(optarg);
        break;
      case 's':
        packet_size = atoi(optarg);
        break;
      default: /* '?' */
        usage();
        break;
    }
  }
  return 0;
}

int main(int argc, char* argv[])
{
  struct timeval t0, t1, t2;
  int r, num;
  unsigned char* buf;
  unsigned char* buf2;
  unsigned char i;
  unsigned int cpt = 0;
  unsigned int wwrite = 0;
  unsigned int wread = 0;
  unsigned int wtotal = 0;

  (void) signal(SIGINT, terminate);
  set_prio();

  int ret;

  read_args(argc, argv);

  if(!baudrate || !samples || !packet_size)
  {
    usage();
    return -1;
  }

  buf = calloc(packet_size, sizeof(*buf));
  buf2 = calloc(packet_size, sizeof(*buf2));

  twrite = calloc(samples, sizeof(*twrite));
  tread = calloc(samples, sizeof(*tread));
  ttotal = calloc(samples, sizeof(*ttotal));

  if(!buf || !buf2 || !twrite || !tread || !ttotal)
  {
    printf("can't allocate memory to store samples!\n");
    return -1;
  }

  int fd1 = serial_connect("/dev/ttyUSB0");
  int fd2 = serial_connect("/dev/ttyUSB1");

  if (fd1 < 0 || fd2 < 0)
  {
    printf("error opening devices\n");
    return -1;
  }

  while (!done)
  {

    for (i = 0; i < packet_size; ++i)
    {
      buf[i] = rand() % 255;
    }

    gettimeofday(&t0, NULL );

    /*
     * send a packet
     */
    ret = write(fd1, buf, packet_size);
    if (ret < 0)
    {
      printf("write error: %s\n", strerror(errno));
      return -1;
    }

    gettimeofday(&t1, NULL );

    /*
     * read a packet
     */
    num = 0;
    while (num != packet_size)
    {
      fd_set rfds;
      FD_ZERO(&rfds);
      FD_SET(fd2, &rfds);
      ret = select(fd2 + 1, &rfds, NULL, NULL, NULL );
      if (ret > 0)
      {
        ret = read(fd2, buf2 + num, packet_size - num);
        if (ret < 1)
        {
          printf("read error: %s\n", strerror(errno));
          return -1;
        }
        num += ret;
      }
      else if(ret < 0 && !done)
      {
        printf("select error: %s\n", strerror(errno));
        return -1;
      }
    }

    gettimeofday(&t2, NULL );

    for (i = 0; i < packet_size; ++i)
    {
      if (buf2[i] != buf[i])
      {
        printf("read error: received=%d, expected=%d\n", buf2[i], buf[i]);
      }
    }

    if(!done)
    {
      twrite[cpt] = (t1.tv_sec * 1000000 + t1.tv_usec)
              - (t0.tv_sec * 1000000 + t0.tv_usec);

      tread[cpt] = (t2.tv_sec * 1000000 + t2.tv_usec)
          - (t1.tv_sec * 1000000 + t1.tv_usec);

      ttotal[cpt] = (t2.tv_sec * 1000000 + t2.tv_usec)
          - (t0.tv_sec * 1000000 + t0.tv_usec);

      if (twrite[cpt] > wwrite)
      {
        wwrite = twrite[cpt];
        //printf("%ld.%06ld worst write: %d\n", t2.tv_sec, t2.tv_usec, wwrite);
      }
      if (tread[cpt] > wread)
      {
        wread = tread[cpt];
        //printf("%ld.%06ld worst read: %d\n", t2.tv_sec, t2.tv_usec, wread);
      }
      if (ttotal[cpt] > wtotal)
      {
        wtotal = ttotal[cpt];
        //printf("%ld.%06ld worst read: %d\n", t2.tv_sec, t2.tv_usec, wtotal);
      }

      cpt++;
      if(cpt == samples)
      {
        break;
      }
    }
  }

  printf("speed: %d\t", rate);
  printf("samples: %d\t", cpt);
  printf("packet size: %d\n", packet_size);
  printf("w:\t\t\tr:\t\t\tw+r\n");
  printf("worst\tavg\tstdev\tworst\tavg\tstdev\tworst\tavg\tstdev\n");
  results(twrite, cpt);
  results(tread, cpt);
  results(ttotal, cpt);
  printf("\n");

  return 0;
}

