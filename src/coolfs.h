#ifndef _COOLFS_H_
#define _COOLFS_H_

#include <fuse.h>
#include <sys/stat.h>

#include "inode.h"

int cool_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi);

int cool_open(const char *path, struct fuse_file_info *fi);

int cool_getattr(const char *path, struct stat *st);

int cool_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi);

#endif /* _COOLFS_H_ */