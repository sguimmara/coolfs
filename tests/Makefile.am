TESTS = check_bitmap check_path check_dir
check_PROGRAMS = check_bitmap check_path check_dir

check_bitmap_SOURCES = check_bitmap.c $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
check_bitmap_CFLAGS = @CHECK_CFLAGS@
check_bitmap_LDADD = @CHECK_LIBS@

check_path_SOURCES = check_path.c $(top_builddir)/src/path.h $(top_builddir)/src/path.c
check_path_CFLAGS = @CHECK_CFLAGS@
check_path_LDADD = @CHECK_LIBS@

check_dir_SOURCES = check_dir.c $(top_builddir)/src/dir.h $(top_builddir)/src/dir.c
check_dir_CFLAGS = @CHECK_CFLAGS@
check_dir_LDADD = @CHECK_LIBS@