GASNET-INSTALL-PATH=/work/XMPTCA/mnakao/work/GASNet-1.24.0
CONDUIT=ibv
include $(GASNET-INSTALL-PATH)/include/$(CONDUIT)-conduit/$(CONDUIT)-seq.mak
CC=mpicc

all: gasnet_put gasnet_get

gasnet_put: gasnet_put.o common.o
	$(GASNET_LD) $(GASNET_LDFLAGS) -o $@ $^ $(GASNET_LIBS)

gasnet_get: gasnet_get.o common.o
	$(GASNET_LD) $(GASNET_LDFLAGS) -o $@ $^ $(GASNET_LIBS)

gasnet_put.o: gasnet_put.c ../common/common.h
	$(GASNET_CC) $(GASNET_CPPFLAGS) $(GASNET_CFLAGS) -I../common -c $<

gasnet_get.o: gasnet_get.c ../common/common.h
	$(GASNET_CC) $(GASNET_CPPFLAGS) $(GASNET_CFLAGS) -I../common -c $<

common.o: ../common/common.c ../common/common.h
#	$(CC) -O2 -c $<
	$(GASNET_CC) $(GASNET_CPPFLAGS) $(GASNET_CFLAGS) -c $<

clean:
	rm -f *.o

