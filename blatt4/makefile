CFLAGS=-Wall -Wextra -pedantic -fopenmp -lm
CC=gcc

parcell: parcell.c
	$(CC) parcell.c -o parcell $(CFLAGS)

.PHONY: clean

clean:
	rm -f parcell
