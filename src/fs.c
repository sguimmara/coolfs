#include <errno.h>

#include "fs.h"
#include "path.h"
#include "inode.h"

void fs_init() {
    inode_init();
    create_filesystem_root();
}

Inode *resolve(const char *path) {
    PathBuf pb;
    parse_path(path, &pb);

    Inode *inode = get_inode_by_path(&pb);

    return inode;
}

int _getattr(const char *path, struct stat *st) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -1;
    }

    stat_inode(inode, st);

    return 0;
}

int _chown(const char *path, uid_t uid, gid_t gid) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -1;
    }

    inode->uid = uid;
    inode->gid = gid;

    return 0;
}

int _chmod(const char *path, mode_t mode) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -1;
    }

    inode->mode = (inode->mode & S_IFMT) | mode;

    return 0;
}

int _rmdir(const char *path) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -1;
    }

    if (inode == get_root()) {
        // Let's be opinionated and declare that we can't delete the root node.
        errno = EACCES;
        return -1;
    }

    remove_entry(parent, inode->number);
    remove_inode(inode->number);
}