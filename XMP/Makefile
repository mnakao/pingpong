CC=xmpcc -O2 -I../common

all: xmp_put xmp_get

xmp_put: xmp_put.o common.o
	$(CC) -o $@ $^

xmp_get: xmp_get.o common.o
	$(CC) -o $@ $^

xmp_put.o: xmp_put.c ../common/common.h
	$(CC) -c $<

xmp_get.o: xmp_get.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

clean:
	rm -f *.o
