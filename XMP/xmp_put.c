#include <stdio.h>
#include <xmp.h>
#include "common.h"

#pragma xmp nodes p(2)
char src_buf[MAX_SIZE]:[*], dst_buf[MAX_SIZE]:[*];

int main(){
  double t;
  int i, me, target, tag = 1;
  unsigned int size;

  me = xmp_node_num();
  target = 3 - me;

  init_buf(src_buf, me);
  init_buf(dst_buf, me);

  if(me==1) print_items();

  for(size=8;size<MAX_SIZE+1;size*=2){ // size must be more than 4 when using Fujitsu RDMA
    xmp_sync_all(NULL);
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
        t = wtime();

      if(me == 1){
	dst_buf[0:size]:[target] = src_buf[0:size];
	xmp_sync_memory(NULL);
#pragma xmp post(p(2), tag)
#pragma xmp wait(p(2), tag)

#ifndef _DEBUG
	if(dst_buf[0] != '2' && dst_buf[size-1] != '2') fprintf(stderr, "Error !\n");
	dst_buf[0] = '1'; dst_buf[size-1] = '1';
#endif
      }
      else{
#pragma xmp wait(p(1), tag)
#ifndef _DEBUG
	if(dst_buf[0] != '1' && dst_buf[size-1] != '1') fprintf(stderr, "Error !\n");
        dst_buf[0] = '2'; dst_buf[size-1] = '2';
#endif
	dst_buf[0:size]:[target] = src_buf[0:size];
	xmp_sync_memory(NULL);
#pragma xmp post(p(1), tag)
      }
    }

    xmp_sync_all(NULL);
    t = wtime() - t;
    if(me == 1)
      print_results(size, t);
  }

  return 0;
}
