TESTS = bincheck_bitmap bincheck_path bincheck_inode bincheck_fs
check_PROGRAMS = bincheck_bitmap bincheck_path bincheck_inode bincheck_fs

bincheck_bitmap_SOURCES = check_bitmap.c $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
bincheck_bitmap_CFLAGS = @CHECK_CFLAGS@
bincheck_bitmap_LDADD = @CHECK_LIBS@

bincheck_path_SOURCES = check_path.c $(top_builddir)/src/path.h $(top_builddir)/src/path.c
bincheck_path_CFLAGS = @CHECK_CFLAGS@
bincheck_path_LDADD = @CHECK_LIBS@

bincheck_inode_SOURCES = check_inode.c \
                    $(top_builddir)/src/inode.h $(top_builddir)/src/inode.c \
                    $(top_builddir)/src/path.h $(top_builddir)/src/path.c \
                    $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
bincheck_inode_CFLAGS = @CHECK_CFLAGS@
bincheck_inode_LDADD = @CHECK_LIBS@

bincheck_fs_SOURCES = check_fs.c \
                    $(top_builddir)/src/fs.h $(top_builddir)/src/fs.c \
                    $(top_builddir)/src/inode.h $(top_builddir)/src/inode.c \
                    $(top_builddir)/src/path.h $(top_builddir)/src/path.c \
                    $(top_builddir)/src/bitmap.h $(top_builddir)/src/bitmap.c
bincheck_fs_CFLAGS = @CHECK_CFLAGS@
bincheck_fs_LDADD = @CHECK_LIBS@