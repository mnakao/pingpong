CC=mpicc -O2 -I../common

all: mpi2_put mpi2_get

mpi2_put: mpi2_put.o common.o
	$(CC) -o $@ $^

mpi2_get: mpi2_get.o common.o
	$(CC) -o $@ $^

mpi2_put.o: mpi2_put.c ../common/common.h
	$(CC) -c $<

mpi2_get.o: mpi2_get.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
