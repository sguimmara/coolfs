#include "config.h"

#ifndef _COOL_DIRENT_H_
#define _COOL_DIRENT_H_

#include <sys/types.h>
#include <sys/stat.h>

#include "block.h"
#include "path.h"

#define MAX_PATH 255
#define MAX_ENTRIES 255

/**
 * @brief A directory entry.
 */
typedef struct DirEnt {
    ino_t inode;
    char name[MAX_PATH];
} DirEnt;

/**
 * @brief A directory inode.
 */
typedef struct DirInode {
    size_t entry_count;
    DirEnt entries[MAX_ENTRIES];
} DirInode;

typedef struct FileInode {
    size_t size;
    blkcnt_t block_count;
    blno_t blocks[8];
} FileInode;

typedef struct Inode {
    ino_t number;
    mode_t mode;

    uid_t uid;
    gid_t gid;

    time_t ctime;
    time_t atime;
    time_t mtime;

    union {
        DirInode dir;
        FileInode file;
    } data;
} Inode;

void inode_init();
void inode_destroy();

Inode *get_root();

/**
 * @brief Get the inode by its number.
 *
 * @param ino The inode number.
 * @return Inode* The inode, otherwise NULL if no inode is found.
 */
Inode *get_inode(ino_t ino);

/**
 * @brief Removes the inode from the filesystem.
 *
 * @param ino
 */
void remove_inode(const ino_t ino);

/**
 * @brief Get the inode by path.
 *
 * @param path The path to the inode.
 * @return Inode* The inode if found, otherwise NULL, and sets errno with the
 * appropriate error code.
 */
Inode* get_inode_by_pathbuf(const PathBuf *path);

int get_inode_and_parent_by_pathbuf(const PathBuf *path,
                                 Inode **inode, Inode** parent);

/**
 * @brief Create a filesystem root, with UID and GID both to 0.
 *
 * @return Inode*
 */
Inode *create_filesystem_root();

Inode *create_directory(const Inode* parent);

Inode *create_file(const Inode* parent);

/**
 * @brief Add a directory entry to the parent inode.
 *
 * @return int zero if success, ENOTDIR if parent is not a directory.
 */
int add_entry(Inode* parent, const char *name, ino_t inode);

int remove_entry(Inode *parent, ino_t inode);

/**
 * @brief Returns the status of inode.
 */
void stat_inode(const Inode *inode, struct stat *st);

#endif /* _COOL_DIRENT_H_ */
