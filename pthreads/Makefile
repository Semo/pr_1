CFLAGS=-Wall -Wpedantic -pthread -lm
CC=gcc

all:	montecarlo

montecarlo: montecarlopi.c
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f montecarlo