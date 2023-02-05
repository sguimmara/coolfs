CC = gcc
LIBFUSE ?= $(shell pkg-config --cflags --libs fuse)
LIBS = $(LIBFUSE)
DEFINES = -D_POSIX_SOURCE -DLOG_USE_COLOR
C_STD = -std=c18
FLAGS = -Wall -g
OPTS = $(FLAGS) $(C_STD) $(DEFINES)
RM = rm -f
BIN = coolfs
INSTALL_PREFIX = /usr/bin

OBJS=main.o coolfs.o inode.o log/log.o

default: $(BIN).out

coolfs.o: coolfs.c coolfs.h inode.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

inode.o: inode.c inode.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

main.o: main.c inode.h coolfs.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

log/log.o: log/log.c
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

$(BIN).out: $(OBJS)
	$(CC) $(OPTS) $^ -o $@ $(LIBS)

clean:
	$(RM) *.o
	$(RM) *.out

install: build
	cp $(BIN).out $(INSTALL_PREFIX)/$(BIN)
