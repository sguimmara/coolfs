TESTS = bincheck_bitmap bincheck_path bincheck_inode bincheck_fs
check_PROGRAMS = bincheck_bitmap bincheck_path bincheck_inode bincheck_fs

bincheck_bitmap_SOURCES = check_bitmap.c
bincheck_bitmap_CFLAGS = @CHECK_CFLAGS@
bincheck_bitmap_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@

bincheck_path_SOURCES = check_path.c
bincheck_path_CFLAGS = @CHECK_CFLAGS@
bincheck_path_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@

bincheck_inode_SOURCES = check_inode.c
bincheck_inode_CFLAGS = @CHECK_CFLAGS@
bincheck_inode_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@

bincheck_fs_SOURCES = check_fs.c
bincheck_fs_CFLAGS = @CHECK_CFLAGS@
bincheck_fs_LDADD = $(top_builddir)/src/libcoolfs.la @CHECK_LIBS@