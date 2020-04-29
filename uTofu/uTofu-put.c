#include <stdlib.h>
#include <assert.h>
#include <mpi.h>
#include <utofu.h>
#include "common.h"
char send_buffer[MAX_SIZE];
volatile char recv_buffer[MAX_SIZE];

// 送信と送信完了確認
static void send(utofu_vcq_hdl_t vcq_hdl, utofu_vcq_id_t rmt_vcq_id, utofu_stadd_t lcl_send_stadd, 
		 utofu_stadd_t rmt_recv_stadd, size_t length, uint64_t edata, 
		 uintptr_t cbvalue, unsigned long int post_flags)
{
  int rc;

  // Put通信を指示
  utofu_put(vcq_hdl, rmt_vcq_id, lcl_send_stadd, rmt_recv_stadd, length,
	    edata, post_flags, (void *)cbvalue);

  // TCQ通知を確認
  if (post_flags & UTOFU_ONESIDED_FLAG_TCQ_NOTICE) {
    void *cbdata;
    do {
      rc = utofu_poll_tcq(vcq_hdl, 0, &cbdata);
    } while (rc == UTOFU_ERR_NOT_FOUND);
    assert(rc == UTOFU_SUCCESS); 
    assert((uintptr_t)cbdata == cbvalue);
  }

  // ローカルMRQ通知を確認
  if (post_flags & UTOFU_ONESIDED_FLAG_LOCAL_MRQ_NOTICE) {
    struct utofu_mrq_notice notice;
    do {
      rc = utofu_poll_mrq(vcq_hdl, 0, &notice);
    } while (rc == UTOFU_ERR_NOT_FOUND);
    assert(rc == UTOFU_SUCCESS); 
    assert(notice.notice_type == UTOFU_MRQ_TYPE_LCL_PUT); 
    assert(notice.edata == edata);
  }
}

// 受信確認
static void recv(utofu_vcq_hdl_t vcq_hdl, volatile char *recv_buffer, 
		 size_t len, uint64_t expected_value,
		 uint64_t edata, unsigned long int post_flags)
{
  int rc;

  // リモートMRQ通知またはメモリ更新を確認
  if (post_flags & UTOFU_ONESIDED_FLAG_REMOTE_MRQ_NOTICE) {
    struct utofu_mrq_notice notice; 
    do {
      rc = utofu_poll_mrq(vcq_hdl, 0, &notice);
    } while (rc == UTOFU_ERR_NOT_FOUND);
    assert(rc == UTOFU_SUCCESS); 
    assert(notice.notice_type == UTOFU_MRQ_TYPE_RMT_PUT);
    assert(notice.edata == edata);
    assert(recv_buffer[0] == expected_value);
    assert(recv_buffer[len-1] == expected_value);
  } 
  else {
    while (recv_buffer[0] != expected_value);
    while (recv_buffer[len-1] != expected_value);
  }
}

int main(int argc, char *argv[]) {
  int rc, num_processes, lcl_rank, rmt_rank; 
  unsigned long int post_flags;
  double t;
  size_t num_tnis, length = MAX_SIZE;
  uint64_t edata;
  uintptr_t cbvalue;
  utofu_tni_id_t tni_id, *tni_ids;
  utofu_vcq_hdl_t vcq_hdl;
  utofu_vcq_id_t lcl_vcq_id, rmt_vcq_id;
  utofu_stadd_t lcl_send_stadd, lcl_recv_stadd, rmt_recv_stadd; 
  struct utofu_onesided_caps *onesided_caps;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processes); 
  MPI_Comm_rank(MPI_COMM_WORLD, &lcl_rank);
  rmt_rank = (lcl_rank == 0) ? 1 : 0;

  // ワンサイド通信に使用可能なTNIのIDを取得
  rc = utofu_get_onesided_tnis(&tni_ids, &num_tnis); 
  if (rc != UTOFU_SUCCESS || num_tnis == 0) {
    MPI_Abort(MPI_COMM_WORLD, 1);
    return 1;
  }
  tni_id = tni_ids[0]; 
  free(tni_ids);

  // ワンサイド通信に関するTNIの機能を問い合わせ
  utofu_query_onesided_caps(tni_id, &onesided_caps);
  
  // VCQを作成して対応するVCQ IDを取得 
  utofu_create_vcq(tni_id, 0, &vcq_hdl); 
  utofu_query_vcq_id(vcq_hdl, &lcl_vcq_id);

  // VCQにメモリを登録して対応するSTADDを取得 
  utofu_reg_mem(vcq_hdl, (void *)send_buffer, length, 0, &lcl_send_stadd);
  utofu_reg_mem(vcq_hdl, (void *)recv_buffer, length, 0, &lcl_recv_stadd);

  // VCQ IDとSTADDを通信相手プロセスに通知
  MPI_Sendrecv(&lcl_vcq_id, 1, MPI_UINT64_T, rmt_rank, 0,
	       &rmt_vcq_id, 1, MPI_UINT64_T, rmt_rank, 0,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE); 
  MPI_Sendrecv(&lcl_recv_stadd, 1, MPI_UINT64_T, rmt_rank, 0, 
	       &rmt_recv_stadd, 1, MPI_UINT64_T, rmt_rank, 0,
	       MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // 通知されたVCQ IDにデフォルト通信経路座標を埋め込み 
  utofu_set_vcq_id_path(&rmt_vcq_id, NULL);

  // リモートMRQ通知を使ってping-pong通信 post_flags = UTOFU_ONESIDED_FLAG_TCQ_NOTICE |
 post_flags = UTOFU_ONESIDED_FLAG_TCQ_NOTICE |
              UTOFU_ONESIDED_FLAG_REMOTE_MRQ_NOTICE | 
              UTOFU_ONESIDED_FLAG_LOCAL_MRQ_NOTICE;

 for(int size=1;size<MAX_SIZE+1;size*=2){
   recv_buffer[0] = recv_buffer[size-1] = 0;
   send_buffer[0] = send_buffer[size-1] = 1;
   MPI_Barrier(MPI_COMM_WORLD);
   // iterationごとにedataの値を変えて、sendで実行したTOQに対応するrecvのMRQを識別 
   for (int i = 0; i < LOOP+WARMUP; i++) {
     if(WARMUP == i)
       t = wtime();
     // 使用できるedataは8bytesなので、256回ごとに0回にリセット
     edata = i % (1UL << (8 * onesided_caps->max_edata_size)); 
     cbvalue = i;
     if (lcl_rank == 0) {
       send(vcq_hdl, rmt_vcq_id, lcl_send_stadd, rmt_recv_stadd, size,
	    edata, cbvalue, post_flags);
       recv(vcq_hdl, recv_buffer, size, 1, edata, post_flags);
       recv_buffer[0] = recv_buffer[size-1] = 0;
     } else {
       recv(vcq_hdl, recv_buffer, size, 1, edata, post_flags); 
       recv_buffer[0] = recv_buffer[size-1] = 0;
       send(vcq_hdl, rmt_vcq_id, lcl_send_stadd, rmt_recv_stadd, size,
	    edata, cbvalue, post_flags);
     }
   }
   MPI_Barrier(MPI_COMM_WORLD);
   t = wtime() - t;
   if(lcl_rank == 0)
     print_results(size, t);
 }

 /*
 recv_buffer[0]     = UINT64_MAX; 
 recv_buffer[len-1] = UINT64_MAX;
 MPI_Barrier(MPI_COMM_WORLD);

 // メモリ更新を使ってping-pong通信 
 post_flags = UTOFU_ONESIDED_FLAG_TCQ_NOTICE |
              UTOFU_ONESIDED_FLAG_LOCAL_MRQ_NOTICE;

 // iterationごとにedataの値を変えて、sendで実行したTOQに対応するrecvのMRQを識別 
 for (i = 0; i < iteration; i++) {
   // 使用できるedataは8bytesなので、256回ごとに0回にリセット 
   edata = i % (1UL << (8 * onesided_caps->max_edata_size));
   cbvalue = i;
   send_buffer = i;
   if (lcl_rank == 0) {
     send(vcq_hdl, rmt_vcq_id, lcl_send_stadd, rmt_recv_stadd, length,
	  edata, cbvalue, post_flags);
     recv(vcq_hdl, &recv_buffer, send_buffer, edata, post_flags); 
     recv_buffer = UINT64_MAX;
   } else {
     recv(vcq_hdl, &recv_buffer, send_buffer, edata, post_flags);
     recv_buffer = UINT64_MAX;
     send(vcq_hdl, rmt_vcq_id, lcl_send_stadd, rmt_recv_stadd, length,
	  edata, cbvalue, post_flags);
   }
 }
 */

 // 資源を開放
 utofu_dereg_mem(vcq_hdl, lcl_send_stadd, 0);
 utofu_dereg_mem(vcq_hdl, lcl_recv_stadd, 0); 
 utofu_free_vcq(vcq_hdl);

 MPI_Finalize();

 return 0; 
}
