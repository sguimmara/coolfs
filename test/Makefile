TESTS = storage.test inode.test
LIBFUSE ?= $(shell pkg-config --cflags --libs fuse)
LIBS = $(LIBFUSE) libcoolfs.so
CC = gcc
OPTS = -Wall -g
MAKE = make -q

OBJS = ../src/inode.o ../src/storage.o ../src/log.o

default: test

test.o: test.c
	$(CC) $(OPTS) -c -g $< -o $@

.PHONY: inode.test
inode.test: test.o inode.test.c
	@$(MAKE) -C ../src minimal
	$(CC) $(OPTS) $^ $(OBJS) -o $@

.PHONY: storage.test
storage.test: test.o storage.test.c
	@$(MAKE) -C ../src minimal
	$(CC) $(OPTS) $^ $(OBJS) -o $@

.PHONY: test
test: $(TESTS)
	@echo ==================== BEGIN TESTS ===================================
	@for t in $(TESTS); do export LD_LIBRARY_PATH=`pwd`; ./$$t; done;
	@echo ===================== END TESTS ====================================
