#include "config.h"

#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "log/log.h"

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

    if (inode) {
        log_debug("[%s] %s -> inode %lu", __FUNCTION__, path, inode->number);
    }

    return inode;
}

int _getattr(const char *path, struct stat *st) {
    log_debug("[%s] %s...", __FUNCTION__, path);
    Inode *inode = resolve(path);

    if (!inode) {
        return -errno;
    }

    stat_inode(inode, st);

    log_debug("[%s] %s -> inode %lu", __FUNCTION__, path, inode->number);

    return 0;
}

int _chown(const char *path, uid_t uid, gid_t gid) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -errno;
    }

    inode->uid = uid;
    inode->gid = gid;

    return 0;
}

int _chmod(const char *path, mode_t mode) {
    Inode *inode = resolve(path);

    if (!inode) {
        return -errno;
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
        return -EACCES;
    }

    if (!S_ISDIR(inode->mode)) {
        return -ENOTDIR;
    }

    if (inode->data.dir.entry_count != 0) {
        return -ENOTEMPTY;
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
        return -errno;
    }

    if (S_ISDIR(inode->mode)) {
        return -EISDIR;
    }

    remove_entry(parent, inode->number);
    remove_inode(inode->number);

    return 0;
}

int _access(const char *path, int mode) {
    log_debug("[%s] %s (%o)", __FUNCTION__, path, mode);
    Inode * inode = resolve(path);

    if (!inode) {
        return -ENOENT;
    }

    if (mode == F_OK) {
        return 0;
    }

    // TODO implement proper access() logic (see access(2))
    return -ENODEV;
}

struct stat *get_stat(const Inode *inode) {
    struct stat *st = malloc(sizeof(struct stat));
    stat_inode(inode, st);
    return st;
}

int _create(const char *path, mode_t mode) {
    PathBuf pb;
    parse_path(path, &pb);
    const char *base = basename(&pb);
    pb.count--;

    log_debug("[%s] %s (%o) (basename: %s)", __FUNCTION__, path, mode, base);

    Inode *parent = get_inode_by_path(&pb);

    if (!parent) {
        log_debug("[%s] %s parent: %p", __FUNCTION__, path, parent);
        return -errno;
    }

    if (!S_ISDIR(parent->mode)) {
        return -ENOTDIR;
    }

    if (S_ISDIR(mode) == 1) {
        Inode *dir = create_directory(parent);
        add_entry(parent, base, dir->number);
        return 0;
    } else if (S_ISREG(mode) == 1) {
        Inode *f = create_file(parent);
        add_entry(parent, base, f->number);
        return 0;
    }

    log_error("[%s] -> not implemented: mode %o", mode);
    return -1;
}

int _readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset) {

    PathBuf pb;
    parse_path(path, &pb);

    Inode *inode;
    Inode *parent;

    if (get_inode_and_parent_by_path(&pb, &inode, &parent) != 0) {
        return -errno;
    }

    if (!inode) {
        return -ENOENT;
    }

    if (!S_ISDIR(inode->mode)) {
        return -ENOTDIR;
    }

    struct stat *stthis = get_stat(inode);
    filler(buf, ".", stthis, 0);
    free(stthis);

    struct stat *stparent = get_stat(parent != NULL ? parent : inode);
    filler(buf, "..", stparent, 0);
    free(stparent);

    DirInode dir = inode->data.dir;
    for (size_t i = 0; i < dir.entry_count; i++) {
        DirEnt ent = dir.entries[i];
        struct stat *st = get_stat(get_inode(ent.inode));
        filler(buf, ent.name, st, 0);
        free(st);
    }

    return 0;
}
