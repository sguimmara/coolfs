#ifndef _COOL_FS_H_
#define _COOL_FS_H_

#include "../config.h"

#include <sys/stat.h>

void fs_init();

int _getattr(const char *path, struct stat *st);

int _chown(const char *path, uid_t uid, gid_t gid);

int _chmod(const char *path, mode_t mode);

int _rmdir(const char *path);

#endif /* _COOL_FS_H_ */