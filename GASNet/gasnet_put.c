#include <stdio.h>
#include <gasnet.h>
#include "common.h"

#define GASNET_BARRIER() do {  \
  gasnet_barrier_notify(0,GASNET_BARRIERFLAG_ANONYMOUS); \
  gasnet_barrier_wait(0,GASNET_BARRIERFLAG_ANONYMOUS);   \
  } while (0)

int main(int argc, char **argv){
  int i;
  unsigned int size;
  double t, t_max;

  gasnet_init(&argc, &argv);
  gasnet_node_t me = gasnet_mynode();
  gasnet_node_t target = 1 - me;
  gasnet_node_t comm_size = gasnet_nodes();

  if(comm_size != 2){
    if(me == 0)
      fprintf(stderr, "WARNING: This program must be executed by two nodes\n");
    gasnet_exit(1);
  }

  gasnet_attach(NULL, 0, MAX_SIZE*sizeof(char), 0);

  gasnet_seginfo_t *s = malloc(comm_size*sizeof(gasnet_seginfo_t));
  char **buf = malloc(sizeof(char*) * comm_size);
  gasnet_getSegmentInfo(s, comm_size);
  for(i=0;i<comm_size;i++)
    buf[i] =  (char*)s[i].addr;

  init_buf(buf[me], me);

  if(me==0) print_items();
  
  for(size=1;size<MAX_SIZE+1;size*=2){
    GASNET_BARRIER();
    for(i=0;i<LOOP+WARMUP;i++){
      if(WARMUP == i)
	t = wtime();

      if(me == 0){
	gasnet_put_bulk(target, buf[target], buf[me], size);
	GASNET_BLOCKUNTIL(buf[me][0] == '1');
	GASNET_BLOCKUNTIL(buf[me][size-1] == '1');
        buf[me][0] = '0';
	buf[me][size-1] = '0';
      } 
      else {
	GASNET_BLOCKUNTIL(buf[me][0] == '0');
	GASNET_BLOCKUNTIL(buf[me][size-1] == '0');
        buf[me][0] = '1'; 
	buf[me][size-1] = '1';
	gasnet_put_bulk(target, buf[target], buf[me], size);
      } 
    } //end of LOOP

    GASNET_BARRIER();
    t = wtime() - t;
    if(me == 0)
      print_results(size, t);
  }

  gasnet_exit(0);

  return 0;
}
