#include <stdio.h>
#include <mpi.h>
#include "common.h"
char send_buf[MAX_SIZE], recv_buf[MAX_SIZE];

int main(int argc, char **argv){
  int i, me, target;
  unsigned int size;
  double t;
  MPI_Request req;
  MPI_Status s;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  target = 1 - me;

  init_buf(send_buf, me);
  init_buf(recv_buf, target);

  if(me==0) print_items();

  for(size=1;size<MAX_SIZE+1;size*=2){
    MPI_Barrier(MPI_COMM_WORLD);
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
	t = wtime();

      if(me == 0){
	MPI_Isend(send_buf, size, MPI_CHAR, target, 9, MPI_COMM_WORLD, &req);
	MPI_Wait(&req, &s);
	MPI_Irecv(recv_buf, size, MPI_CHAR, target, 5, MPI_COMM_WORLD, &req);
	MPI_Wait(&req, &s);
      } 
      else {
	MPI_Irecv(recv_buf, size, MPI_CHAR, target, 9, MPI_COMM_WORLD, &req);
	MPI_Wait(&req, &s);
	MPI_Isend(send_buf, size, MPI_CHAR, target, 5, MPI_COMM_WORLD, &req);
	MPI_Wait(&req, &s);
      } 
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t = wtime() - t;
    if(me == 0)
      print_results(size, t);
  }

  MPI_Finalize();
  return 0;
}
