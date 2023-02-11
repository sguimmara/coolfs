#ifndef _COOL_DIR_H_
#define _COOL_DIR_H_

#include <sys/types.h>
#include <sys/stat.h>

/**
 * @brief A directory entry.
 */
typedef struct Dirent {
    char *name;
    ino_t ino;
    mode_t mode;
    size_t size;
    uid_t uid;
    gid_t gid;
    time_t ctime;
    time_t atime;
    time_t mtime;

    union {
        struct Dir_entries  *entries;
    } node;
} Dirent;

/**
 * @brief The children of a directory.
 * 
 */
typedef struct Dir_entries {
    size_t entry_cnt;
    struct Dirent **entries;
} Dir_entries;

// struct stat *cl_get_stat(Dirent *dirent);
// Dirent *cl_new_root();
// cool_dir *cl_new_dir();
// cool_freg *cl_new_file(ino_t inode);
// Dirent *cl_add_file(cool_dir *dir, char *name, cool_freg* file);
// Dirent *cl_add_dir(cool_dir *dir, char *name, cool_dir* subdir);
// void cl_remove_entry(cool_dir *dir, Dirent *entry);
// Dirent *cl_get_dirent(Dirent *parent, const char *name);

// void cl_print_root(cool_dir* root);
// void cl_print_dirent(Dirent *ent, int level);

#endif /* _COOL_DIR_H_ */