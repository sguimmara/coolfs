#include "config.h"

#ifdef HAVE_LIBFUSE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "log/log.h"

#include "fuseimpl.h"
#include "bitmap.h"
#include "block_allocator.h"
#include "constants.h"
#include "inode.h"
#include "fs.h"
#include "path.h"

void fuse_init() {
    fs_init();
}

void fuse_deinit(void *unknown) {
    fs_destroy();
}

inline int checked(const char *fname, const char *path, int ret) {
    if (ret != 0) {
        int err = errno;
        log_debug("%s: %s -> %s (%d)", fname, path, strerror(-ret), ret);
    }

    return ret;
}

int fuse_getattr(const char *path, struct stat *st) {
    return checked(__FUNCTION__, path, _getattr(path, st));
}

int fuse_access(const char *path, int mode) {
    return checked(__FUNCTION__, path, _access(path, mode));
}

int fuse_chmod(const char *path, mode_t mode) {
    return checked(__FUNCTION__, path, _chmod(path, mode));
}

int fuse_chown(const char *path, uid_t uid, gid_t gid) {
    return checked(__FUNCTION__, path, _chown(path, uid, gid));
}

int fuse_unlink(const char *path) {
    return checked(__FUNCTION__, path, _unlink(path));
}

int fuse_rmdir(const char *path) {
    return checked(__FUNCTION__, path, _rmdir(path));
}

int fuse_readdir(const char *path,
                 void *buf,
                 fuse_fill_dir_t filler,
                 off_t offset,
                 struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _readdir(path, buf, filler, offset));
}

int fuse_open(const char *path, struct fuse_file_info *fi) {
    // TODO what should we do here ? see open(2)
    return 0;
}

int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _create(path, mode));
}

int fuse_rename(const char *src, const char *dst) {
    return checked(__FUNCTION__, src, _rename(src, dst));
}

int fuse_mkdir(const char *path, mode_t mode) {
    return checked(__FUNCTION__, path, _mkdir(path, mode));
}

int fuse_utimens(const char *path, const struct timespec tv[2]) {
    return checked(__FUNCTION__, path, _utimens(path, tv));
}

int fuse_write(const char *path, const char *buf, size_t size, off_t offset,
               struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _write(path, buf, size, offset));
}

int fuse_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _read(path, buf, size, offset));
}

#endif /* HAVE_LIBFUSE */
