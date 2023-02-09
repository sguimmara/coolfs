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

INCLUDES=fs.h block_allocator.h inode.h bitmap.h dir.h inode_alloc.h constants.h
OBJS=fs.o block_allocator.o inode.o log.o bitmap.o dir.o inode_alloc.o

default: coolfs mkcoolfs coolfsck

log.o: log/log.c
	$(CC) $(OPTS) -c $< -o $@

block_allocator.o: block_allocator.c block_allocator.h constants.h
	$(CC) $(OPTS) -c $< -o $@

inode.o: inode.c inode.h constants.h
	$(CC) $(OPTS) -c $< -o $@

inode_alloc.o: inode_alloc.c inode_alloc.h constants.h
	$(CC) $(OPTS) -c $< -o $@

dir.o: dir.c dir.h constants.h
	$(CC) $(OPTS) -c $< -o $@

bitmap.o: bitmap.c bitmap.h constants.h
	$(CC) $(OPTS) -c $< -o $@
 
coolfs.o: coolfs.c inode.h fs.h block_allocator.h constants.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

fs.o: fs.c fs.h inode.h constants.h
	$(CC) $(OPTS) -c $< -o $@ $(LIBS)

minimal: inode.o block_allocator.o log.o

lib$(NAME).so: $(OBJS) $(INCLUDES)
	$(CC) $(OPTS) -shared $^ -o $@ $(LIBS)

mkcoolfs: mkcoolfs.c $(OBJS)
	$(CC) $(OPTS) $^ -o $@ $(LIBS)

coolfsck: coolfsck.c $(OBJS)
	$(CC) $(OPTS) $^ -o $@ $(LIBS)

coolfs: coolfs.c $(OBJS)
	$(CC) $(OPTS) $^ -o $@ $(LIBS)

clean:
	$(RM) *.o
	$(RM) *.out
	$(RM) *.so
	$(RM) coolfs
	$(RM) mkcoolfs

install: build
	cp $(NAME).out $(INSTALL_PREFIX)/$(NAME)
