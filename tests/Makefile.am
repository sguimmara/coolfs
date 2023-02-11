TESTS = bincheck_bitmap bincheck_path bincheck_dir
check_PROGRAMS = bincheck_bitmap bincheck_path bincheck_dir

bincheck_bitmap_SOURCES = check_bitmap.c $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
bincheck_bitmap_CFLAGS = @CHECK_CFLAGS@
bincheck_bitmap_LDADD = @CHECK_LIBS@

bincheck_path_SOURCES = check_path.c $(top_builddir)/src/path.h $(top_builddir)/src/path.c
bincheck_path_CFLAGS = @CHECK_CFLAGS@
bincheck_path_LDADD = @CHECK_LIBS@

bincheck_dir_SOURCES = check_dir.c $(top_builddir)/src/dir.h $(top_builddir)/src/dir.c \
                    $(top_builddir)/src/path.h $(top_builddir)/src/path.c
bincheck_dir_CFLAGS = @CHECK_CFLAGS@
bincheck_dir_LDADD = @CHECK_LIBS@