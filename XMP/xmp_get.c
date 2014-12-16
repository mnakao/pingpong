#include <stdio.h>
#include <xmp.h>
#include "common.h"

#pragma xmp nodes p(2)
char local_buf[MAX_SIZE], target_buf[MAX_SIZE];
#pragma xmp coarray local_buf:[*]
#pragma xmp coarray target_buf:[*]

int main(){
  double t;
  int i, me, target;
  unsigned int size;

  me = xmp_node_num();
  target = 3 - me;

  init_buf(local_buf, me);
  init_buf(target_buf, me);

  if(me==1) print_items();

  for(size=1;size<MAX_SIZE+1;size*=2){
    xmp_sync_all(NULL);
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
        t = wtime();

      if(me == 1){
        local_buf[0:size] = target_buf[0:size]:[target];
	while(local_buf[0] == '1'){
	  xmp_sync_memory(NULL);
	}
	while(local_buf[size-1] == '1'){
	  xmp_sync_memory(NULL);
	}
	local_buf[0] = '1'; local_buf[size-1] = '1';
	xmp_sync_all(NULL);
      }
      else{
	xmp_sync_all(NULL);
	local_buf[0:size] = target_buf[0:size]:[target];

	while(local_buf[0] == '2'){
          xmp_sync_memory(NULL);
        }
        while(local_buf[size-1] == '2'){
          xmp_sync_memory(NULL);
	}
        local_buf[0] = '2'; local_buf[size-1] = '2';
      }
      xmp_sync_all(NULL);
    }

    xmp_sync_all(NULL);
    t = wtime() - t;
    if(me == 1)
      print_results(size, t);
  }

  return 0;
}
