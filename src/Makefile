CC = gcc
LIBFUSE ?= $(shell pkg-config --cflags --libs fuse)
LIBS = $(LIBFUSE)
DEFINES = -D_POSIX_SOURCE=200809L -DLOG_USE_COLOR
C_STD = -std=c18
FLAGS = -Wall -g -fPIC
OPTS = $(FLAGS) $(C_STD) $(DEFINES)
RM = rm -f
NAME = coolfs
INSTALL_PREFIX = /usr/bin

OBJS=fs.o storage.o inode.o log.o

default: $(NAME).out

log.o: log/log.c
	$(CC) $(OPTS) -c $< -o $@

storage.o: storage.c storage.h
	$(CC) $(OPTS) -c $< -o $@

inode.o: inode.c inode.h
	$(CC) $(OPTS) -c $< -o $@

main.o: main.c inode.h fs.h storage.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

fs.o: fs.c fs.h inode.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

minimal: inode.o storage.o log.o

lib$(NAME).so: $(OBJS)
	$(CC) $(OPTS) -shared $^ -o $@ $(LIBS)

$(NAME).out: main.o $(OBJS)
	$(CC) $(OPTS) $^ -o $@ $(LIBS)

clean:
	$(RM) *.o
	$(RM) *.out
	$(RM) *.so

install: build
	cp $(NAME).out $(INSTALL_PREFIX)/$(NAME)
