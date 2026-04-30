CC=gcc
CFLAGS=-Iinclude -Wall -O2

SRC=$(wildcard src/*.c)

all:
	mkdir -p bin
	$(CC) $(SRC) $(CFLAGS) -o bin/compressor

clean:
	rm -rf bin/*.o bin/compressor
	rm -rf /build/test.cmp
