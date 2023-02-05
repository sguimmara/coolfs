#define _XOPEN_SOURCE 1

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "fs.h"
#include "inode.h"

#include "log/log.h"

static cool_inode *root;

static char *PATH_SEP = "/";

cool_inode *mk_root() {
    log_trace("mk_root: making root node");

    cool_inode *res = mk_dir(1, "/");

    root = res;

    return res;
}

cool_inode *cool_find_inode(const char *path) {
    assert(root != NULL);
    assert(path != NULL);

    log_debug("cool_find_inode: %s", path);

    if (strcmp(path, PATH_SEP) == 0) {
        log_trace("cool_find_inode: returning root node");
        return root;
    }

    cool_inode *cur = root;
    char *cpy = malloc(strlen(path) + 1);
    strcpy(cpy, path);

    char *fragment = strtok(cpy, PATH_SEP);

    while (fragment) {
        cur = get_child(cur, fragment);
        if (cur == NULL) {
            return NULL;
        }

        return cur;
        fragment = strtok(NULL, PATH_SEP);
        if (fragment == NULL) {
            log_debug("cool_find_inode: %s -> inode %s", path, cur->name);
            return cur;
        }
    }

    return NULL;
}

int init(FILE *file) { return 0; }

int cool_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
    log_debug("[read] %s (bufsize: %zo)", path, size);

    cool_inode *inode = cool_find_inode(path);

    if (inode != NULL) {
        off_t size = inode->st->st_size;
        log_trace("[read] copying content of %lo bytes", size);
        inode_read(buf, inode, size, offset);
        return size;
    }

    return -1;
}

int cool_open(const char *path, struct fuse_file_info *fi) {
    log_debug(path);
    fi->fh = 10;
    return 0;
}

int cool_getattr(const char *path, struct stat *st) {
    log_debug("[getattr] %s", path);

    memset(st, 0, sizeof(struct stat));

    cool_inode *inode = cool_find_inode(path);

    if (inode != NULL) {
        memcpy(st, inode->st, sizeof(struct stat));
        log_debug("[getattr] %s -> success", path);
        return 0;
    }

    log_debug("[getattr] %s -> ENOENT", path);
    return -ENOENT;
}

int cool_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
    (void)offset;
    (void)fi;

    log_trace("[readdir] %s", path);

    cool_inode *inode = cool_find_inode(path);

    if (inode == NULL) {
        log_trace("[readdir] %s -> ENOENT", path);
        return -ENOENT;
    }

    log_trace("[readdir] %s (%lo children)", inode->name,
              inode->children->count);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (size_t i = 0; i < inode->children->count; i++) {
        cool_inode *child = inode->children->elems[i];
        filler(buf, child->name, child->st, 0);
    }

    return 0;
}