#include <stdlib.h>

#include "dir.h"

int compare_dirents(const void *dira, const void *dirb) {
    cool_dirent *a = (cool_dirent *)dira;
    cool_dirent *b = (cool_dirent *)dirb;

    if (a->is_dir == b->is_dir) {
        return 0;
    }

    if (a->is_dir && !b->is_dir) {
        return -1;
    }

    return 1;
}

cool_dir *cool_mkdir() {
    cool_dir *res = malloc(sizeof(cool_dir));
    res->entries = NULL;
    res->entry_cnt = 0;
    return res;
}

cool_dirent *dir_add_inode(cool_dir *dir, char *name, cool_inode *inode) {
    dir->entry_cnt++;
    size_t new_size = dir->entry_cnt * sizeof(cool_dirent*);
    if (dir->entries == NULL) {
        dir->entries = malloc(new_size);
    } else {
        dir->entries = realloc(dir->entries, new_size);
    }

    cool_dirent *new_entry = malloc(sizeof(cool_dirent));
    new_entry->is_dir = 0;
    new_entry->name = name;
    new_entry->node.inode = inode;

    dir->entries[dir->entry_cnt - 1] = new_entry;

    qsort(dir->entries, dir->entry_cnt, sizeof(cool_dirent*), compare_dirents);

    return new_entry;
}

cool_dirent *dir_add_dir(cool_dir *dir, char *name, cool_dir *subdir) {
    dir->entry_cnt++;
    size_t new_size = dir->entry_cnt * sizeof(cool_dirent*);
    if (dir->entries == NULL) {
        dir->entries = malloc(new_size);
    } else {
        dir->entries = realloc(dir->entries, new_size);
    }

    cool_dirent *new_entry = malloc(sizeof(cool_dirent));
    new_entry->is_dir = 1;
    new_entry->name = name;
    new_entry->node.dir = subdir;

    dir->entries[dir->entry_cnt - 1] = new_entry;

    qsort(dir->entries, dir->entry_cnt, sizeof(cool_dirent*), compare_dirents);

    return new_entry;
}

void print_root(cool_dir* root) {
    printf("/\n");
    for (size_t i = 0; i < root->entry_cnt; i++) {
        print_dir(root->entries[i], 1);
    }
}

void print_dir(cool_dirent *ent, int level) {
    char *indent = malloc(level * 2 + 1);
    for (size_t i = 0; i < (level * 2); i++) {
        indent[i] = ' ';
    }

    indent[level * 2] = '\0';

    if (ent->is_dir) {
        printf("%s%s/\n", indent, ent->name);
    } else {
        printf("%s%s\n", indent, ent->name);
    }

    if (ent->is_dir) {
        cool_dir *dir = ent->node.dir;
        for (size_t i = 0; i < dir->entry_cnt; i++) {
            if (dir->entries[i]->is_dir) {
                print_dir(dir->entries[i], level + 1);
            } else {
                printf("%s%s\n", indent, dir->entries[i]->name);
            }
        }
    }
}
