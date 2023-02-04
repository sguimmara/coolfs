CC = gcc
OPTS = -Wall -g -std=c18 -D_POSIX_SOURCE
RM = rm -f
BIN = coolfs

default: build

build:
	$(CC) $(OPTS) main.c `pkg-config fuse --cflags --libs` -o $(BIN).out

clean:
	$(RM) main.o
	$(RM) coolfs
