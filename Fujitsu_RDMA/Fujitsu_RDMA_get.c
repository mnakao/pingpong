#include <stdio.h>
#include <mpi.h>
#include <mpi-ext.h>
#include "common.h"

#define FLAG_NIC (FJMPI_RDMA_LOCAL_NIC0 | FJMPI_RDMA_REMOTE_NIC1 | FJMPI_RDMA_IMMEDIATE_RETURN)
#define SEND_NIC FJMPI_RDMA_LOCAL_NIC0
char buf[MAX_SIZE];
struct FJMPI_Rdma_cq cq;

int main(int argc, char *argv[]){
  int i, me, target;
  unsigned int size;
  uint64_t laddr, raddr;
  double t;

  MPI_Init(&argc, &argv);
  FJMPI_Rdma_init();
  MPI_Comm_rank(MPI_COMM_WORLD, &me);
  target = 1 - me;

  init_buf(buf, me);

  laddr = FJMPI_Rdma_reg_mem(0, buf, sizeof(char)*MAX_SIZE);
  while((raddr = FJMPI_Rdma_get_remote_addr(target, 0)) == FJMPI_RDMA_ERROR);

  if(me==0) print_items();

  for(size=4;size<MAX_SIZE+1;size*=2){  // size must be more than 4
    MPI_Barrier(MPI_COMM_WORLD);

    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
        t = wtime();

      if(me == 0){
	FJMPI_Rdma_get(target, 0, raddr, laddr, size, FLAG_NIC);
	while(buf[0] == '0' || buf[size-1] == '0'){
          FJMPI_Rdma_poll_cq(SEND_NIC, &cq);
        }
        buf[0] = buf[size-1] = '0';
	MPI_Barrier(MPI_COMM_WORLD);
      }
      else{
	MPI_Barrier(MPI_COMM_WORLD);
	FJMPI_Rdma_get(target, 0, raddr, laddr, size, FLAG_NIC);
	while(buf[0] == '1' || buf[size-1] == '1'){
          FJMPI_Rdma_poll_cq(SEND_NIC, &cq);
        }
        buf[0] = buf[size-1] = '1';
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    t = wtime() - t;
    if(me == 0)
      print_results(size, t);
  }

  FJMPI_Rdma_finalize();
  MPI_Finalize();
  return 0;
}
