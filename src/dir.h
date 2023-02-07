#ifndef _COOL_DIR_H_
#define _COOL_DIR_H_

#include <sys/types.h>
#include <sys/stat.h>
#include "inode.h"

struct cool_dir;

/**
 * @brief A directory entry.
 */
typedef struct cool_dirent {
    char *name;
    int type;

    union {
        struct cool_dir  *dir;
        struct cool_freg *file;
    } node;
} cool_dirent;

/**
 * @brief A directory.
 * 
 */
typedef struct cool_dir {
    struct stat *stat;
    struct cool_dirent **entries;
    size_t entry_cnt;
} cool_dir;

/**
 * @brief A regular file.
 * 
 */
typedef struct cool_freg {
    ino_t inode;
} cool_freg;


struct stat *cl_get_stat(cool_dirent *dirent);
cool_dirent *cl_new_root();
cool_dir *cl_new_dir();
cool_freg *cl_new_file(ino_t inode);
cool_dirent *cl_add_file(cool_dir *dir, char *name, cool_freg* file);
cool_dirent *cl_add_dir(cool_dir *dir, char *name, cool_dir* subdir);
cool_dirent *cl_get_dirent(cool_dirent *parent, const char *name);

void cl_print_root(cool_dir* root);
void cl_print_dirent(cool_dirent *ent, int level);

#endif /* _COOL_DIR_H_ */