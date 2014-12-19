CC=mpifccpx -Kfast -I../common
FC=mpifrtpx -Kfast -I../common

ifeq ($(LANG),Fortran)
PROG=K_F_put K_F_get
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

clean:
	rm -f *.o
