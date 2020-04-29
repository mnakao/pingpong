CC=mpifccpx -Kfast -I../common

uTofu-put: uTofu-put.o common.o
	$(CC) -o $@ $^ -ltofucom

uTofu-put.o: uTofu-put.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
