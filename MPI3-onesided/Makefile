CC=mpicc -O2 -I../common

all: mpi3_put mpi3_get

mpi3_put: mpi3_put.o common.o
	$(CC) -o $@ $^

mpi3_get: mpi3_get.o common.o
	$(CC) -o $@ $^

mpi3_put.o: mpi3_put.c ../common/common.h
	$(CC) -c $<

mpi3_get.o: mpi3_get.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
