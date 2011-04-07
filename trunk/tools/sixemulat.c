/*
 * Compile with: gcc -lm -o sixemulat sixemulat.c
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>

char mouse[32];
char sixemu[32];
char usbmon[] = "/sys/kernel/debug/usb/usbmon/0u";

#define RECORDS 1024

unsigned int tdiff[RECORDS] = {0};

void results()
{
  int i;
  int val;
  int sum = 0;
  int average;
  int temp = 0;
  int nbval;
  int worst = 0;
  
  for (i = 0; i<RECORDS && tdiff[i]>0; i++)
  {
    sum = sum+tdiff[i];
    if(tdiff[i] > worst)
    {
      worst = tdiff[i];
    }
  }
  
  nbval = i;

  printf("nb: %d\n", nbval);
  printf("worst: %d\n", worst);

  if(nbval < 2)
  {
    return;
  }

  average = sum / nbval;

  printf("average: %d\n", average);
  
  for (i = 0; i < nbval; i++)
  {
    temp = temp + pow(abs(tdiff[i]-average), 2);
  }
  
  temp = pow(temp/(nbval-1), 0.5);

  printf("standard deviation: %d\n", temp);
}

static int bexit = 0;

void ex_program(int sig)
{
  bexit = 1;
}

void usage()
{
  printf("usage: ./sixemulat <mouse bus id> <mouse device id> <dongle bus id> <dongle device id>\n");
  exit(-1);
}

int main(int argc, char *argv[])
{
  int i = 0;
  FILE* fp;
  char line[LINE_MAX];
  unsigned int tclic = 0;
  unsigned int tr1 = 0;
  char test[128];
  unsigned char done = 0;
  int mbid;
  int dbid;
  int mid;
  int did;
  
  (void) signal(SIGINT, ex_program);

  if(argc > 1)
  {
    mbid = atoi(argv[1]);
    if(mbid < 1 || mbid > 9)
    {
      usage();
    }   
  }
  else
  {
    usage();
  }

  if(argc > 2)
  {
    mid = atoi(argv[2]);
    if(mid < 1 || did > 128)
    {
      usage();
    }
  }
  else
  {
    usage();
  }

  if(argc > 3)
  {
    dbid = atoi(argv[3]);
    if(dbid < 1 || dbid > 9)
    {
      usage();
    }
  }
  else
  {
    usage();
  }

  if(argc > 4)
  {
    did = atoi(argv[4]);
    if(did < 1 || did > 128)
    {
      usage();
    }
  }
  else
  {
    usage();
  }

  sprintf(mouse, "Ii:%d:%03d", mbid, mid);
  sprintf(sixemu, "Bo:%d:%03d", dbid, did);

  fp = fopen(usbmon, "r");
  if (!fp)
  {
    fprintf(stderr, "Can not open file\n");
  }
  else
  {
    while (fgets(line, LINE_MAX, fp) && !bexit)
    {
      if(strstr(line, mouse))
      {
        /*
         * Crude heuristic to find left clics.
         */
        if(strstr(line, "= 01"))
        {
          if(!tclic && !done)
          {
            sscanf(line, "%*[^ ]%*[ ]%10u", &tclic);
          }
        }
        else if(strstr(line, "= 00"))
        {
          tclic = 0;
          done = 0;
        }
      }
      else if(strstr(line, sixemu))
      {
        /*
         * Crude heuristic to find r1 button presses.
         */
        if(strstr(line, "0000ff00"))
        {
          if(tclic && !done)
          {
            sscanf(line, "%*[^ ]%*[ ]%10u", &tr1);
            if(i < RECORDS)
            {
              tdiff[i] = tr1 - tclic;
              printf("latency: %d\n", tdiff[i]);
              i++;
            }
            else
            {
              printf("max records reached!\n");
              break;
            }
            done = 1;
          }
        }
      }
    }
    fclose(fp);
  }

  results();

  return 0;
}

