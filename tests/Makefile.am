TESTS = bincheck
check_PROGRAMS = bincheck

bincheck_SOURCES = run_tests.c check_all.h \
                   check_block_allocator.c \
                   check_bitmap.c \
                   check_path.c \
                   check_fs.c \
                   check_serializer.c \
                   check_inode.c

bincheck_CFLAGS = @CHECK_CFLAGS@
bincheck_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@
