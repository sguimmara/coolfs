#include "config.h"

#ifndef _COOLFS_H_
#define _COOLFS_H_

#ifdef HAVE_LIBFUSE

#include <fuse.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>

#include "inode.h"

void fuse_init();
int fuse_getattr(const char *path, struct stat *st);
int fuse_access(const char *path, int mode);
int fuse_chmod(const char *path, mode_t mode);
int fuse_chown(const char *path, uid_t uid, gid_t gid);
int fuse_unlink(const char *path);
int fuse_rmdir(const char *path);
int fuse_readdir(const char *path,
                 void *buf,
                 fuse_fill_dir_t filler,
                 off_t offset,
                 struct fuse_file_info *fi);
int fuse_open(const char *path, struct fuse_file_info *fi);
int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi);
int fuse_rename(const char* src, const char *dst);
int fuse_mkdir(const char *path, mode_t mode);
int fuse_utimens(const char *path, const struct timespec tv[2]);

#endif /* HAVE_LIBFUSE */

#endif /* _COOLFS_H_ */