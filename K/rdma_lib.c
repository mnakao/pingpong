#include <mpi-ext.h>
#include <stdio.h>

/* データポーリング */
int flag_nic0 = FJMPI_RDMA_LOCAL_NIC0 | FJMPI_RDMA_REMOTE_NIC0 | FJMPI_RDMA_PATH0; // | FJMPI_RDMA_REMOTE_NOTICE;
int flag_nic1 = FJMPI_RDMA_LOCAL_NIC1 | FJMPI_RDMA_REMOTE_NIC1 | FJMPI_RDMA_PATH1; // | FJMPI_RDMA_REMOTE_NOTICE;
int flag_nic2 = FJMPI_RDMA_LOCAL_NIC2 | FJMPI_RDMA_REMOTE_NIC2 | FJMPI_RDMA_PATH2; // | FJMPI_RDMA_REMOTE_NOTICE;
int flag_nic3 = FJMPI_RDMA_LOCAL_NIC3 | FJMPI_RDMA_REMOTE_NIC3 | FJMPI_RDMA_PATH3; // | FJMPI_RDMA_REMOTE_NOTICE;

void fjmpi_rdma_init_(void){
  FJMPI_Rdma_init();
}

void fjmpi_rdma_finalize_(void){
  FJMPI_Rdma_finalize();
}

uint64_t fjmpi_rdma_reg_mem_(int memid, void *sbuf, size_t length){
  return FJMPI_Rdma_reg_mem(memid, sbuf, length);
}

uint64_t fjmpi_rdma_get_remote_addr_(int pid, int memid){
  uint64_t raddr;
  while((raddr = FJMPI_Rdma_get_remote_addr(pid, memid)) == FJMPI_RDMA_ERROR);
  return raddr;
}

void fjmpi_rdma_put_nic0_(int pid, int tag, uint64_t raddr, uint64_t laddr, size_t length){
  FJMPI_Rdma_put(pid, tag, raddr, laddr, length, flag_nic0);
}

void fjmpi_rdma_put_nic1_(int pid, int tag, uint64_t raddr, uint64_t laddr, size_t length){
  FJMPI_Rdma_put(pid, tag, raddr, laddr, length, flag_nic1);
}

// 完了キューポーティング
void fjmpi_rdma_poll_cq_local_nic0_(void){
  while(FJMPI_Rdma_poll_cq(FJMPI_RDMA_LOCAL_NIC0, NULL) != FJMPI_RDMA_NOTICE);
}

void fjmpi_rdma_poll_cq_local_nic1_(void){
  while(FJMPI_Rdma_poll_cq(FJMPI_RDMA_LOCAL_NIC1, NULL) != FJMPI_RDMA_NOTICE);
}

void fjmpi_rdma_poll_cq_remote_nic1_(void){
  while(FJMPI_Rdma_poll_cq(FJMPI_RDMA_LOCAL_NIC1, NULL) != FJMPI_RDMA_HALFWAY_NOTICE);
}
