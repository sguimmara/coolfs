#include "config.h"

#ifndef _COOLFS_H_
#define _COOLFS_H_

#ifdef HAVE_LIBFUSE

#include <fuse.h>

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

// /**
//  * @brief Initializes a new filesystem into the provided file.
//  *
//  * @param filename The persistent storage file.
//  * @return int The status.
//  */
// int cl_mkfs(const char* filename);

// void cl_fsinit(Inode *root);

// int cl_open_dev(const char *filename);

// int cl_read(const char *path, char *buf, size_t size, off_t offset,
//               struct fuse_file_info *fi);

// int cl_open(const char *path, struct fuse_file_info *fi);

// int cl_getattr(const char *path, struct stat *st);

// int cl_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
//                  off_t offset, struct fuse_file_info *fi);

// int cl_write(const char *path, const char *buf, size_t size,
//              off_t offset, struct fuse_file_info *fi);

// int cl_access(const char *path, int mode);


// int cl_create(const char *path, mode_t mode, struct fuse_file_info *fi);


#endif /* HAVE_LIBFUSE */

#endif /* _COOLFS_H_ */