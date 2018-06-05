CFLAGS=-Wall -Wextra -pedantic -fopenmp -lm
CC=gcc

pi_omp: pi_omp.c
	$(CC) pi_omp.c -o pi_omp $(CFLAGS)

.PHONY: clean

clean:
	rm -f pi_omp
