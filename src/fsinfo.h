#ifndef _COOLFS_FSINFO_H_
#define _COOLFS_FSINFO_H_

#include <stddef.h>
#include <stdint.h>

#include "bitmap.h"
#include "inode.h"

enum {
    FS_NONE = 0,         // No flag
    FS_ENCRYPT = 1 << 0, // Data is encrypted
    FS_COMPRESS = 1 << 1 // Data is compressed
};

typedef struct FilesystemInfo {
    size_t version;
    size_t block_size;
    uint8_t flags;
    size_t block_count;
    size_t inode_count;

    char reserved[512];
} FilesystemInfo;

typedef struct Filesystem {
    FilesystemInfo *info;

    bitmap *inode_bitmap;
    Inode **inode_table;
    bitmap *block_bitmap;
    FILE *storage;
} Filesystem;

#endif /* _COOLFS_FSINFO_H_ */
