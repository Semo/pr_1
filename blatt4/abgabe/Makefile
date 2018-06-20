CC=gcc
CFLAGS=-O2 -fopenmp
LDFLAGS=-lcrypto

.PHONY: clean

all:	camp campi

camp: camp.c random.c md5tool.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

campi: campi.c random.c md5tool.c
	mpicc $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.o
	rm -rf camp
	rm -rf campi
