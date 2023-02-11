AC_INIT([coolfs], [0.1], [])
AM_INIT_AUTOMAKE([subdir-objects foreign -Wall -Werror dist-bzip2 ])

AC_PROG_CC

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_CONFIG_SRCDIR(src/coolfs.c)

AC_CHECK_HEADERS([unistd.h check.h])
AC_CHECK_LIB([fuse], [fuse_main])
PKG_CHECK_MODULES([CHECK], [check >= 0.15.0])

AC_DEFINE([_FILE_OFFSET_BITS], [64], [Required by FUSE])
AC_DEFINE([__USE_XOPEN], [1], [Required for file types (S_IFREG, S_IFDIR)])
AC_DEFINE([_XOPEN_SOURCE], [1], [Required for file types (S_IFREG, S_IFDIR)])
AC_DEFINE([FUSE_USE_VERSION], [26], [Use FUSE 2.6])
AC_DEFINE([LOG_USE_COLOR], [], [Use colored log output]) 

AC_OUTPUT