#include <sys/time.h>
#include <stdio.h>
#include "common.h"

double wtime(void)
{
  static int sec = -1;
  struct timeval tv;
  gettimeofday(&tv, (void *)0);
  if (sec < 0) sec = tv.tv_sec;
  return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

void print_items(void)
{
  printf("    Size\tBandwidth(Byte/s)\t Time(s)\n");
}

void print_results(unsigned int size, double t)
{
  printf("%8d\t%17.0f\t%8.6f\n", size, size/t*LOOP*2, t);
}

void init_buf(char buf[MAX_SIZE], int num)
{
  int i;
  char tmp_buf[2];
  sprintf(tmp_buf, "%d", num);

  for(i=0;i<MAX_SIZE;i++) 
    buf[i] = tmp_buf[0];
}
