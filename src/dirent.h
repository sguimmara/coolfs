#ifndef _COOL_DIRENT_H_
#define _COOL_DIRENT_H_

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>

#include "path.h"

struct Dirent;

/**
 * @brief A directory entry.
 */
typedef struct Dirent {
    char *name;
    mode_t mode;
    ino_t ino;
    size_t size;

    uid_t uid;
    gid_t gid;

    time_t ctime;
    time_t atime;
    time_t mtime;

    /* Hierarchy */
    struct Dirent *parent;
    size_t child_count;
    struct Dirent **children;
} Dirent;

// /**
//  * @brief The children of a directory.
//  * 
//  */
// typedef struct Dir_entries {
//     size_t entry_cnt;
//     struct Dirent **entries;
// } Dir_entries;

/**
 * @brief Creates a new directory with default values.
 * 
 * @param name The name of the entry.
 * @param ino The inode number.
 * @param type The entry type (S_IFDIR, S_IFREG...)
 * @param uid The entry UID
 * @param gid The entry GID
 * @return Dirent* 
 */
Dirent* new_dirent(const char *name, ino_t ino, unsigned int type, uid_t uid, gid_t gid);

/**
 * @brief Returns the status of the directory entry.
 */
void dirent_stat(const Dirent* dirent, struct stat* st);

/**
 * @brief Adds a child to the dirent.
 * 
 * @param parent 
 * @param child 
 * @return int zero if success, ENOTDIR if parent is not a directory.
 */
int add_child(Dirent *parent, Dirent *child);

/**
 * @brief Removes the child from the parent.
 * 
 * @param parent 
 * @param child 
 * @return int 
 */
int remove(Dirent *parent, Dirent *child);

Dirent *find_descendant(Dirent* root, const PathBuf* path);

#endif /* _COOL_DIRENT_H_ */
