#ifndef _COOL_DIR_H_
#define _COOL_DIR_H_

#include "inode.h"

struct cool_dir;

typedef struct cool_dirent {
    char *name;
    int is_dir;

    union {
        struct cool_dir  *dir;
        struct cool_inode *inode;
    } node;
} cool_dirent;

/**
 * @brief A directory.
 * 
 */
typedef struct cool_dir {
    struct cool_dirent **entries;
    size_t entry_cnt;
} cool_dir;

cool_dir *cool_mkdir();
cool_dirent *dir_add_inode(cool_dir *dir, char *name, cool_inode* inode);
cool_dirent *dir_add_dir(cool_dir *dir, char *name, cool_dir* subdir);

void print_root(cool_dir* root);
void print_dir(cool_dirent *ent, int level);

#endif /* _COOL_DIR_H_ */