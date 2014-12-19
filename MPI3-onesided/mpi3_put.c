#include <stdio.h>
#include <mpi.h>
#include "common.h"
char send_buf[MAX_SIZE];

int main(int argc, char **argv){
  int i, me, target;
  unsigned int size;
  double t, t_max;
  MPI_Win win;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  MPI_Win_create(&send_buf, sizeof(char)*MAX_SIZE, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  
  target = 1 - me;
  MPI_Win_lock_all(0, win);
  
  init_buf(send_buf, me);

  if(me==0) print_items();

  for(size=1;size<MAX_SIZE+1;size*=2){
    MPI_Barrier(MPI_COMM_WORLD);
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
	t = wtime();

      if(me == 0){
	MPI_Put(send_buf, size, MPI_CHAR, target, 0, size, MPI_CHAR, win);
	MPI_Win_flush_local(target, win);

	while(send_buf[0] == '0' || send_buf[size-1] == '0'){ MPI_Win_flush(me, win); }
	send_buf[0] = '0'; send_buf[size-1] = '0';
      }
      else {
	while(send_buf[0] == '1' || send_buf[size-1] == '1'){ MPI_Win_flush(me, win); }
	send_buf[0] = '1'; send_buf[size-1] = '1';

	MPI_Put(send_buf, size, MPI_CHAR, target, 0, size, MPI_CHAR, win);
	MPI_Win_flush_local(target, win);
      }
    } //end of LOOP

    t = wtime() - t;
    MPI_Reduce(&t, &t_max, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if(me == 0)
      print_results(size, t_max);
  }

  MPI_Win_unlock_all(win);
  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
}
