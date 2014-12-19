ifeq ($(ARCH),K)
CC=mpifccpx -Kfast -I../common
else
CC=mpicc -O2 -I../common
endif

send_recv: send_recv.o common.o
	$(CC) -o $@ $^

send_recv.o: send_recv.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
