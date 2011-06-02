/*
 * gcc -g -o test test.c -lpthread
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <termios.h>

#define BUFFER_SIZE 48

static int bexit = 0;

static int fd;

static unsigned char expected = 0;

void ex_program(int sig)
{
  bexit = 1;
}

int main(int argc, char *argv[])
{
  struct timeval t0, t1;
  double tsec;
  struct termios options;
  fd_set rfds;
  fd_set wfds;
  struct timeval timeout;
  pthread_t thread;
  pthread_attr_t thread_attr;
  unsigned int cwrite = 0;
  unsigned int cread = 0;
  unsigned char w_data;
  unsigned char r_data;
  unsigned char wbuffer[BUFFER_SIZE];
  unsigned char rbuffer[BUFFER_SIZE];
  int i;
  for(i=0; i<sizeof(wbuffer); ++i)
  {
    wbuffer[i] = i%sizeof(wbuffer);
  }
  (void) signal(SIGINT, ex_program);
  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY/* | O_NONBLOCK*/);
  tcgetattr(fd, &options);
  cfsetispeed(&options, B500000);
  cfsetospeed(&options, B500000);
  options.c_cflag |= (CLOCAL | CREAD);
  tcsetattr(fd, TCSANOW, &options);
  options.c_cflag &= ~PARENB;
  options.c_cflag &= ~CSTOPB;
  options.c_cflag &= ~CSIZE;
  options.c_cflag |= CS8;

  gettimeofday(&t0, NULL);

  while(!bexit)
  {
    w_data = cwrite%sizeof(wbuffer);
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);
    FD_ZERO(&wfds);
    FD_SET(fd, &wfds);
    timeout.tv_sec = 1;//wait up to 1s
    timeout.tv_usec = 0;
    if(select(fd+1, &rfds, &wfds, NULL, &timeout) > 0)
    {
      if(FD_ISSET(fd, &wfds))
      {
        //if(write(fd, &w_data, sizeof(w_data)) == sizeof(w_data))
        if(write(fd, wbuffer, sizeof(wbuffer)) == sizeof(wbuffer))
        {
          //printf("write: %hu\n", w_data);
          cwrite++;
        }
        else
        {
          printf("write error: %hu\n", w_data);
        }
      }
      if(FD_ISSET(fd, &rfds))
      {
        while(read(fd, &r_data, sizeof(r_data)) == sizeof(r_data))
        {
          //printf("read: %hu\n", r_data);
          if(r_data != expected)
          {
            printf("wrong byte: e(%hu) r(%hu)!!\n", expected, r_data);
          }
          else
          {
            cread++;
          }
          expected = r_data+1;
        }
      }
    }
    //usleep(1000);
  }
  gettimeofday(&t1, NULL);
  tsec = ((double)((t1.tv_sec*1000000+t1.tv_usec)-(t0.tv_sec*1000000+t0.tv_usec)))/1000000;
  close(fd);
  printf("cwrite: %d\n", cwrite*sizeof(wbuffer));
  printf("cread: %d\n", cread);
  printf("time: %.2f\n", tsec);
  printf("cwrite: %.2f o/ms\n", cwrite*sizeof(wbuffer)/tsec/1000);
  printf("cread: %.2f o/ms\n", cread/tsec/1000);
  printf("w_data: %d\n", w_data);
  printf("r_data: %d\n", r_data);
}
