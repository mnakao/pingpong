CC=mpicc -O2 -I../common
obj = send_recv.o common.o

send_recv: $(obj)
	$(CC) -o $@ $^

send_recv.o: send_recv.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
