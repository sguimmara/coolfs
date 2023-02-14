#ifndef _COOLFS_BLOCK_H_
#define _COOLFS_BLOCK_H_

#include <stddef.h>

/**
 * @brief A block number
 */
typedef unsigned int blno_t;

typedef unsigned char block_flags;

#define BLCK_CHNG 0001 // Block should be written into storage at next sync

/** A data block */
typedef struct Block {
    blno_t no;
    block_flags flags;
    size_t size;
    char *content;
} Block;

#endif /* _COOLFS_BLOCK_H_ */