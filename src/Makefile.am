lib_LTLIBRARIES = libcoolfs.la
libcoolfs_la_SOURCES = log/log.c \
                       path.c \
                       bitmap.c \
                       fuseimpl.c \
                       fs.c \
                       inode.c \
                       block_allocator.c \
                       serializer.c

bin_PROGRAMS = coolfs
coolfs_SOURCES = coolfs.c
coolfs_LDADD = libcoolfs.la