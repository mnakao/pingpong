NVCC=nvcc
CC=mpicc
MPI=/opt/MPI/MVAPICH2-GDR/2.0/GNU/CUDA/6.5/
OPT=-O2 -I../common -I$(MPI)/include/

cuda_pingpong: cuda_pingpong.cu common.o
	$(NVCC) $^ -o $@ $(OPT) $(MPI)/lib64/libmpich.so

common.o: ../common/common.c ../common/common.h
	$(NVCC) $(OPT) -c $<

clean:
	rm -f *.o submit.sh.*

