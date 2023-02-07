#define _XOPEN_SOURCE 700

#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "log/log.h"
#include "inode_alloc.h"
#include "dir.h"

struct stat *cl_get_stat(cool_dirent *dirent) {
    switch (dirent->type) {
        case S_IFREG:
            cool_inode *inode = cl_get_inode(dirent->node.file->inode);
            return inode->st;
        case S_IFDIR:
            return dirent->node.dir->stat;
    }

    log_error("fatal: trying to get stat on invalid object");
    exit(1);
}

cool_dirent *cl_new_root() {
    cool_dirent *res = malloc(sizeof(cool_dirent));
    res->name = "/";
    res->type = S_IFDIR;
    res->node.dir = cl_new_dir();

    return res;
}

cool_dir *cl_new_dir() {
    cool_dir *res = malloc(sizeof(cool_dir));

    struct stat *st = calloc(1, sizeof(struct stat));
    st->st_uid = 1000;
    st->st_gid = 1000;
    st->st_ino = 0;
    st->st_mode = S_IFDIR | 0444;
    st->st_size = 0;
    time_t t = time(NULL);
    st->st_ctime = t;
    st->st_atime = t;
    st->st_mtime = t;

    res->entries = NULL;
    res->entry_cnt = 0;
    res->stat = st;

    return res;
}

cool_freg *cl_new_file(ino_t inode) {
    cool_freg *res = malloc(sizeof(cool_freg));
    res->inode = inode;
    return res;
}

cool_dirent *cl_add_file(cool_dir *dir, char *name, cool_freg *file) {
    dir->entry_cnt++;
    size_t new_size = dir->entry_cnt * sizeof(cool_dirent*);
    if (dir->entries == NULL) {
        dir->entries = malloc(new_size);
    } else {
        dir->entries = realloc(dir->entries, new_size);
    }

    cool_dirent *new_entry = malloc(sizeof(cool_dirent));
    new_entry->type = (int)S_IFREG;
    new_entry->name = name;
    new_entry->node.file = file;

    dir->entries[dir->entry_cnt - 1] = new_entry;

    return new_entry;
}

cool_dirent *cl_add_dir(cool_dir *dir, char *name, cool_dir *subdir) {
    dir->entry_cnt++;
    size_t new_size = dir->entry_cnt * sizeof(cool_dirent*);
    if (dir->entries == NULL) {
        dir->entries = malloc(new_size);
    } else {
        dir->entries = realloc(dir->entries, new_size);
    }

    cool_dirent *new_entry = malloc(sizeof(cool_dirent));
    new_entry->type = (int)S_IFDIR;
    new_entry->name = name;
    new_entry->node.dir = subdir;

    dir->entries[dir->entry_cnt - 1] = new_entry;

    return new_entry;
}

cool_dirent *cl_get_dirent(cool_dirent *parent, const char *name) {
    if (parent->type == S_IFDIR) {
        cool_dir *dir = parent->node.dir;
        for (size_t i = 0; i < dir->entry_cnt; i++) {

            cool_dirent *child = dir->entries[i];
            if (strcmp(child->name, name) == 0) {
                return child;
            }
        }
    }

    return NULL;
}

void cl_print_root(cool_dir* root) {
    printf("/\n");
    for (size_t i = 0; i < root->entry_cnt; i++) {
        cl_print_dirent(root->entries[i], 1);
    }
}

const char *BLUE = "\x1b[94m";
const char *CLEAR_TERM = "\x1b[0m";

void cl_print_dirent(cool_dirent *ent, int level) {
    const char *name = ent->name;
    char *indent = malloc(level * 2 + 1);
    for (size_t i = 0; i < (level * 2); i++) {
        indent[i] = ' ';
    }

    indent[level * 2] = '\0';

    switch (ent->type) {
        case S_IFDIR:
            printf("%s%s%s%s\n", indent, BLUE, name, CLEAR_TERM);
            break;
        case S_IFREG:
            printf("%s%s (%lu)\n", indent, name, ent->node.file->inode);
            break;
        default:
            printf("%s? %s\n", indent, name);
            break;
    }

    if (ent->type == S_IFDIR) {
        cool_dir *dir = ent->node.dir;
        for (size_t i = 0; i < dir->entry_cnt; i++) {
            cl_print_dirent(dir->entries[i], level + 1);
        }
    }
}
