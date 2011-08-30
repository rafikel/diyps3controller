/*
 * Compile with: gcc -lm -o sixemulat_usb sixemulat_usb.c
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>

char mouse[32];
char joystick[32];
char usbserial[32];
char usbmon[] = "/sys/kernel/debug/usb/usbmon/0u";

#define RECORDS 1024

unsigned int tdiff1[RECORDS] = {0};
unsigned int tdiff2[RECORDS] = {0};
unsigned int tdiff3[RECORDS] = {0};

void results(unsigned int tdiff[RECORDS])
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
  printf("usage: ./sixemulat <mouse bus id> <mouse device id> <usbserial bus id> <usbserial device id> <joystick bus id> <joystick device id>\n");
  exit(-1);
}

int main(int argc, char *argv[])
{
  int i = 0;
  int j = 0;
  FILE* fp;
  char line[LINE_MAX];
  unsigned int tmouse = 0;
  unsigned int tusbserial = 0;
  unsigned int tjoystick = 0;
  int mdown = 0;
  int udown = 0;
  int jdown = 0;
  char test[128];
  unsigned char done1 = 0;
  unsigned char done2 = 0;
  int mbid;
  int dbid;
  int cbid;
  int cdid;
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

  if(argc > 5)
  {
    cbid = atoi(argv[5]);
    if(cbid < 1 || cbid > 9)
    {
      usage();
    }
  }
  else
  {
    usage();
  }

  if(argc > 6)
  {
    cdid = atoi(argv[6]);
    if(cdid < 1 || cdid > 128)
    {
      usage();
    }
  }
  else
  {
    usage();
  }

  sprintf(mouse, ":%d:%03d", mbid, mid);
  sprintf(usbserial, ":%d:%03d", dbid, did);
  sprintf(joystick, ":%d:%03d", cbid, cdid);

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
          if(!mdown)
          {
            sscanf(line, "%*[^ ]%*[ ]%10u", &tmouse);
            mdown = 1;
          }
        }
        else if(strstr(line, "= 00"))
        {
          mdown = 0;
        }
      }
      else if(strstr(line, usbserial))
      {
        /*
         * Crude heuristic to find r1 button presses.
         */
        if(strstr(line, "08002000"))
        {
          if(!udown)
          {
            sscanf(line, "%*[^ ]%*[ ]%10u", &tusbserial);
            udown = 1;
          }
        }
        else if(strstr(line, "08000000"))
        {
          udown = 0;
        }
      }
      else if(strstr(line, joystick))
      {
        /*
         * Crude heuristic to find r1 button presses.
         */
        if(strstr(line, "08002000"))
        {
          if(!jdown)
          {
            sscanf(line, "%*[^ ]%*[ ]%10u", &tjoystick);
            jdown = 1;
          }
        }
        else if(strstr(line, "08000000"))
        {
          jdown = 0;
        }
      }

      if(tmouse && tusbserial && tjoystick)
      {
        tdiff1[i] = tusbserial - tmouse;
        tdiff2[i] = tjoystick - tusbserial;
        tdiff3[i] = tjoystick - tmouse;
        printf("tmouse: %d\n", tmouse);
        printf("tusbserial: %d\n", tusbserial);
        printf("tjoystick: %d\n", tjoystick);
        printf("latency 1: %d\n", tdiff1[i]);
        printf("latency 2: %d\n", tdiff2[i]);
        printf("latency 3: %d\n", tdiff3[i]);
        i++;
        if(i == RECORDS)
        {
          printf("max records reached!\n");
          break;
        }
        tmouse = 0;
        tusbserial = 0;
        tjoystick = 0;
      }


//      if(strstr(line, mouse))
//      {
//        /*
//         * Crude heuristic to find left clics.
//         */
//        if(strstr(line, "= 01"))
//        {
//          if(!tmouse && !done1 && !done2)
//          {
//            sscanf(line, "%*[^ ]%*[ ]%10u", &tmouse);
//          }
//        }
//        else if(strstr(line, "= 00"))
//        {
//          tmouse = 0;
//          tusbserial = 0;
//          tjoystick = 0;
//          done1 = 0;
//          done2 = 0;
//        }
//      }
//      else if(strstr(line, usbserial))
//      {
//        /*
//         * Crude heuristic to find r1 button presses.
//         */
//        if(strstr(line, "08002000"))
//        {
//          if(tmouse && !done1)
//          {
//            sscanf(line, "%*[^ ]%*[ ]%10u", &tusbserial);
//            if(i < RECORDS)
//            {
//              tdiff1[i] = tusbserial - tmouse;
//              printf("latency 1: %d\n", tdiff1[i]);
//              i++;
//            }
//            else
//            {
//              printf("max records reached!\n");
//              break;
//            }
//            done1 = 1;
//          }
//        }
//      }
//      else if(strstr(line, joystick))
//      {
//        printf("%s\n", line);
//        /*
//         * Crude heuristic to find r1 button presses.
//         */
//        if(strstr(line, "08002000"))
//        {
//          if(tusbserial && !done2)
//          {
//            sscanf(line, "%*[^ ]%*[ ]%10u", &tjoystick);
//            if(j < RECORDS)
//            {
//              tdiff2[j] = tjoystick - tusbserial;
//              printf("latency 2: %d\n", tdiff2[j]);
//              tdiff3[j] = tdiff1[j] + tdiff2[j];
//              printf("sum: %d\n", tdiff3[j]);
//              j++;
//            }
//            else
//            {
//              printf("max records reached!\n");
//              break;
//            }
//            done2 = 1;
//          }
//        }
//      }
    }
    fclose(fp);
  }

  printf("mouse to usbserial:\n");
  results(tdiff1);
  printf("usbserial to joystick:\n");
  results(tdiff2);
  printf("mouse to joystick:\n");
  results(tdiff3);

  return 0;
}

