CC=mpifccpx -O2 -I../common
FC=mpifrtpx -O2 -I../common

ifeq ($(LANG),F)
PROG=K_F_put
else
PROG=K_C_put K_C_get
endif

all: $(PROG)

K_C_put: K_C_put.o common.o
	$(CC) -o $@ $^

K_C_get: K_C_get.o common.o
	$(CC) -o $@ $^

K_C_put.o: K_C_put.c ../common/common.h
	$(CC) -c $<

K_C_get.o: K_C_get.c ../common/common.h
	$(CC) -c $<

common.o: ../common/common.c ../common/common.h
	$(CC) -c $<

K_F_put: K_F_put.o rdma_interface.o
	$(FC) -o $@ $^

rdma_lib.o: rdma_interface.c
	$(CC) -c $^

K_F_put.o: K_F_put.f90
	$(FC) -c $^

clean:
	rm -f *.o
