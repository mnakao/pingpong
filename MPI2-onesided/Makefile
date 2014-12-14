CC=mpicc -O2 -I../common
obj = put_mpi2.o common.o

all: put_mpi2 get_mpi2

put_mpi2: put_mpi2.o common.o
	$(CC) -o $@ $^

get_mpi2: get_mpi2.o common.o
	$(CC) -o $@ $^

put_mpi2.o: put_mpi2.c ../common/common.h
	$(CC) -c $<

get_mpi2.o: get_mpi2.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
