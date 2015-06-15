#include <mpi.h>
#include <stdio.h>
extern "C" {
#include "common.h"
}

char send_buf[MAX_SIZE], recv_buf[MAX_SIZE];

int main(int argc, char** argv)
{
  int i, me, target;
  unsigned int size;
  double t;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &me);

  target = 1 - me;
  init_buf(send_buf, me);
  init_buf(recv_buf, target);
  
  char *send_buf_d, *recv_buf_d;
  cudaMalloc((void **)&send_buf_d, MAX_SIZE);
  cudaMalloc((void **)&recv_buf_d, MAX_SIZE);

  // Initialize
  cudaMemcpy(send_buf_d, send_buf, MAX_SIZE, cudaMemcpyHostToDevice);
  cudaMemcpy(recv_buf_d, recv_buf, MAX_SIZE, cudaMemcpyHostToDevice);

  if(me==0) print_items();

  for(size=1;size<MAX_SIZE+1;size*=2){
    MPI_Barrier(MPI_COMM_WORLD);
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
        t = wtime();

      if(me == 0){
        MPI_Send(send_buf_d, size, MPI_CHAR, target, 9, MPI_COMM_WORLD);
        MPI_Recv(recv_buf_d, size, MPI_CHAR, target, 5, MPI_COMM_WORLD, &status);
      }
      else {
        MPI_Recv(recv_buf_d, size, MPI_CHAR, target, 9, MPI_COMM_WORLD, &status);
        MPI_Send(send_buf_d, size, MPI_CHAR, target, 5, MPI_COMM_WORLD);
      }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t = wtime() - t;
    if(me == 0)
      print_results(size, t);
  }

  cudaFree(send_buf_d);
  cudaFree(recv_buf_d);

  MPI_Finalize();
  return 0;
}

