#include <errno.h>
#include <unistd.h>

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
    PathBuf pb;
    parse_path(path, &pb);

    Inode *inode;
    Inode *parent;

    if (get_inode_and_parent_by_path(&pb, &inode, &parent) != 0) {
        return -1;
    }

    if (inode == get_root()) {
        // Let's be opinionated and declare that we can't delete the root node.
        errno = EACCES;
        return -1;
    }

    if (!S_ISDIR(inode->mode)) {
        errno = ENOTDIR;
        return -1;
    }

    if (inode->data.dir.entry_count != 0) {
        errno = ENOTEMPTY;
        return -1;
    }

    remove_entry(parent, inode->number);
    remove_inode(inode->number);

    return 0;
}

int _unlink(const char *path) {
    PathBuf pb;
    parse_path(path, &pb);

    Inode *inode;
    Inode *parent;

    if (get_inode_and_parent_by_path(&pb, &inode, &parent) != 0) {
        return -1;
    }

    if (S_ISDIR(inode->mode)) {
        errno = EISDIR;
        return -1;
    }

    remove_entry(parent, inode->number);
    remove_inode(inode->number);

    return 0;
}

int _access(const char *path, int mode) {
    Inode * inode = resolve(path);

    if (!inode) {
        return -1;
    }

    if (mode == F_OK) {
        return 0;
    }

    // TODO implement proper access() logic (see access(2))
    errno = ENODEV;
    return -1;
}
