CC = gcc
OPTS = -Wall -std=c18
RM = rm -f

default: build

build: main.o

main.o: main.c
	$(CC) $(OPTS) main.c `pkg-config fuse --cflags --libs` -o coolfs

clean:
	$(RM) main.o
	$(RM) coolfs
