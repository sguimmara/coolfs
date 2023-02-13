TESTS = run_tests
check_PROGRAMS = run_tests

run_tests_SOURCES = run_tests.c check_all.h \
                   check_block_allocator.c \
                   check_bitmap.c \
                   check_path.c \
                   check_fs.c \
                   check_inode.c

run_tests_CFLAGS = @CHECK_CFLAGS@
run_tests_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@
