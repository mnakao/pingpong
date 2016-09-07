ifeq ($(ARCH),K)
CC=mpifccpx -Kfast -I../common
else
CC=mpicc -O2 -I../common
endif

isend_irecv: isend_irecv.o common.o
	$(CC) -o $@ $^

isend_irecv.o: isend_irecv.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
