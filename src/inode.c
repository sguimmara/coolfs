#include "config.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "bitmap.h"
#include "inode.h"
#include "log/log.h"

#define INODE_TABLE_SIZE 2048

Inode *root;
bitmap *inode_bitmap;
// TODO dynamically resize the inode array
Inode *inodes[INODE_TABLE_SIZE];

void inode_init() {
    inode_bitmap = bm_alloc(INODE_TABLE_SIZE);
}

void free_inode(Inode* inode) {
    free(inode);
}

void remove_inode(const ino_t ino) {
    Inode *inode = get_inode(ino);
    assert(inode != NULL);

    free_inode(inode);
    bm_unset(inode_bitmap, ino);
    inodes[ino] = NULL;
}

Inode *get_root() {
    return root;
}

ino_t get_ino_t() {
    size_t number;
    // check that we have not reached the max capacity of inodes
    assert(bm_get(inode_bitmap, &number) == 0);
    return (ino_t)number;
}

Inode *create_filesystem_root() {
    Inode *result = malloc(sizeof(Inode));

    result->number = get_ino_t();

    result->gid = 0;
    result->uid = 0;

    time_t now = time(NULL);

    result->atime = now;
    result->ctime = now;
    result->mtime = now;

    result->mode = S_IFDIR |
        S_IRUSR | S_IWUSR | S_IXUSR |
        S_IRGRP |           S_IXGRP |
        S_IROTH |           S_IXOTH;

    DirInode dir = {
        .entry_count = 0,
    };
    result->data.dir = dir;

    inodes[result->number] = result;

    root = result;

    return result;
}

Inode *create_file(const Inode* parent) {
    Inode *result = malloc(sizeof(Inode));

    result->number = get_ino_t();

    result->gid = parent->gid;
    result->uid = parent->uid;

    time_t now = time(NULL);

    result->atime = now;
    result->ctime = now;
    result->mtime = now;

    result->mode = S_IFREG;

    FileInode file = {
        .size = 0,
        .block_count = 0,
    };
    result->data.file = file;

    inodes[result->number] = result;

    return result;
}

Inode *create_directory(const Inode* parent) {
    Inode *result = malloc(sizeof(Inode));

    result->number = get_ino_t();

    result->gid = parent->gid;
    result->uid = parent->uid;

    time_t now = time(NULL);

    result->atime = now;
    result->ctime = now;
    result->mtime = now;

    result->mode = S_IFDIR;

    DirInode dir = {
        .entry_count = 0,
    };
    result->data.dir = dir;

    inodes[result->number] = result;

    return result;
}

int add_entry(Inode* parent, const char *name, ino_t inode) {
    if (S_ISDIR(parent->mode) != 1) {
        return ENOTDIR;
    }

    DirEnt *entries = parent->data.dir.entries;
    DirEnt new = { .inode = inode };

    // Truncate name to MAX_PATH if necessary
    // TODO add unit test for name truncation
    size_t namelen = strnlen(name, MAX_PATH);
    size_t len = namelen > MAX_PATH ? MAX_PATH : namelen;
    strncpy(new.name, name, len);

    entries[parent->data.dir.entry_count] = new;
    parent->data.dir.entry_count++;

    return 0;
}

int entry_index(const Inode *parent, ino_t ino) {
    DirInode dir = parent->data.dir;
    for (size_t i = 0; i < dir.entry_count; i++) {
        if (dir.entries[i].inode == ino) {
            return i;
        }
    }

   return -1;
}

int remove_entry(Inode *parent, ino_t inode) {
    if (S_ISDIR(parent->mode) != 1) {
        return ENOTDIR;
    }

    int idx = entry_index(parent, inode);

    DirEnt last = parent->data.dir.entries[parent->data.dir.entry_count - 1];
    parent->data.dir.entries[idx] = last;
    parent->data.dir.entry_count--;

    return 0;
}

Inode *get_inode(ino_t ino) {
    if (bm_is_set(inode_bitmap, ino) == 0) {
        return NULL;
    }

    return inodes[ino];
}

int get_inode_and_parent_by_path(const PathBuf *path,
                                 Inode **inode, Inode** parent) {
    if (path->count == 0) {
        *inode = root;
        *parent = NULL;
        return 0;
    }

    Inode *current = root;
    Inode *prnt = NULL;

    errno = 0;

    for (size_t i = 0; i < path->count; i++) {
        const char *fragment = path->fragments[i];
        if (!S_ISDIR(current->mode)) {
            errno = ENOTDIR;
            return -1;
        }
        if (current->data.dir.entry_count > 0) {
            for (size_t i = 0; i < current->data.dir.entry_count; i++) {
                if (strcmp(current->data.dir.entries[i].name, fragment) == 0) {
                    prnt = current;
                    current = get_inode(current->data.dir.entries[i].inode);
                    break;
                }
            }
        } else {
            errno = ENOENT;
            return -1;
        }
    }

    *inode = current;
    *parent = prnt;
    return 0;
}

Inode* get_inode_by_path(const PathBuf *path) {
    Inode *result;
    Inode *parent;

    int ret = get_inode_and_parent_by_path(path, &result, &parent);

    if (ret != 0) {
        return NULL;
    }

    return result;
}

void stat_inode(const Inode *inode, struct stat *st) {
    assert(inode != NULL);
    assert(st != NULL);

    log_debug("[%s] inode %lu", __FUNCTION__, inode->number);

    st->st_ino = inode->number;
    st->st_atime = inode->atime;
    st->st_ctime = inode->ctime;
    st->st_mtime = inode->mtime;
    st->st_mode = inode->mode;
    st->st_uid = inode->uid;
    st->st_gid = inode->gid;
    st->st_nlink = 1; // TODO

    if (S_ISREG(inode->mode)) {
        st->st_size = inode->data.file.size;
    } else {
        st->st_size = 0;
    }
}
