TESTS = bincheck_bitmap bincheck_path bincheck_dirent
check_PROGRAMS = bincheck_bitmap bincheck_path bincheck_dirent

bincheck_bitmap_SOURCES = check_bitmap.c $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
bincheck_bitmap_CFLAGS = @CHECK_CFLAGS@
bincheck_bitmap_LDADD = @CHECK_LIBS@

bincheck_path_SOURCES = check_path.c $(top_builddir)/src/path.h $(top_builddir)/src/path.c
bincheck_path_CFLAGS = @CHECK_CFLAGS@
bincheck_path_LDADD = @CHECK_LIBS@

bincheck_dirent_SOURCES = check_dirent.c $(top_builddir)/src/dirent.h $(top_builddir)/src/dirent.c \
                    $(top_builddir)/src/path.h $(top_builddir)/src/path.c
bincheck_dirent_CFLAGS = @CHECK_CFLAGS@
bincheck_dirent_LDADD = @CHECK_LIBS@