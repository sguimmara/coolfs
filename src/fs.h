#include "config.h"

#ifndef _COOL_FS_H_
#define _COOL_FS_H_

#include <time.h>
#include <sys/stat.h>

typedef int (*fuse_fill_dir_t) (void *buf, const char *name,
				const struct stat *stbuf, off_t off);

void fs_init();

void fs_destroy();

int _getattr(const char *path, struct stat *st);

int _chown(const char *path, uid_t uid, gid_t gid);

int _chmod(const char *path, mode_t mode);

int _rmdir(const char *path);

int _unlink(const char *path);

int _access(const char *path, int mode);

int _create(const char *path, mode_t mode);

int _readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset);

int _rename(const char *src, const char *dst);

int _mkdir(const char *path, mode_t mode);

int _utimens(const char *path, const struct timespec tv[2]);

int _write(const char *path, const char *buf, size_t size, off_t offset);

int _read(const char *path, const char *buf, size_t size, off_t offset);

#endif /* _COOL_FS_H_ */