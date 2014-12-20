CC=mpifccpx -O2 -I../common
PROG=Fujitsu_RDMA_put Fujitsu_RDMA_get

all: $(PROG)

Fujitsu_RDMA_put: Fujitsu_RDMA_put.o common.o
	$(CC) -o $@ $^

Fujitsu_RDMA_get: Fujitsu_RDMA_get.o common.o
	$(CC) -o $@ $^

Fujitsu_RDMA_put.o: Fujitsu_RDMA_put.c ../common/common.h
	$(CC) -c $<

Fujitsu_RDMA_get.o: Fujitsu_RDMA_get.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
