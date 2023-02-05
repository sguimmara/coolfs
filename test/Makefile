TESTS = storage.test inode.test
LIBFUSE ?= $(shell pkg-config --cflags --libs fuse)
LIBS = $(LIBFUSE) libcoolfs.so

default: run

test.o: test.c
	gcc -c -g $< -o $@

.PHONY: lib
lib:
	@make -C ../src libcoolfs.so
	@cp ../src/libcoolfs.so .

.PHONY: inode.test
inode.test: test.o inode.test.c
	@make lib
	gcc -g $^ -o $@ $(LIBS)

.PHONY: storage.test
storage.test: test.o storage.test.c
	@make lib
	gcc -g $^ -o $@ $(LIBS)

run: $(TESTS)
	@for t in $(TESTS); do export LD_LIBRARY_PATH=`pwd`; ./$$t; done;
