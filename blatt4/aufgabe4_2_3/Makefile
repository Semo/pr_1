CC=gcc
CFLAGS=-O2 -fopenmp
LDFLAGS=-lcrypto

.PHONY: clean

all:	caseq caseqMPI

caseq: caseq.c random.c md5tool.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@
	
	
caseqMPI: caseqMPI.c random.c md5tool.c
	$(CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

clean:
	rm -rf *.o
	rm -rf caseq
	rm -rf caseqMPI