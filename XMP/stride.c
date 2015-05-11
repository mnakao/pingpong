#include <stdio.h>
#include <sys/time.h>
#include <xmp.h>
#define WARMUP 100
#define LOOP 1000
#define N 64
#pragma xmp nodes p(2)
double a[N][N][N]:[*];

double wtime(void)
{
  static int sec = -1;
  struct timeval tv;
  gettimeofday(&tv, (void *)0);
  if (sec < 0) sec = tv.tv_sec;
  return (tv.tv_sec - sec) + 1.0e-6*tv.tv_usec;
}

int main()
{
  double t;

  for(int i=0;i<LOOP+WARMUP;i++){
    if(WARMUP == i)
      t = wtime();

    xmp_sync_all(NULL);
    if(xmp_node_num() == 1){
      a[0:N][0:N][0]:[2] = a[0:N][0:N][0];
      xmp_sync_memory(NULL);
    }
  }
  t = wtime() - t;

  if(xmp_node_num() == 1)
    printf("%.3f sec. (%.3f MB/s)\n", t, LOOP*N*N*sizeof(double)/1024/1024/t);

  return 0;
}
