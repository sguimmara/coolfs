#ifndef _COOLFS_BLOCK_H_
#define _COOLFS_BLOCK_H_

#include <stddef.h>

/**
 * @brief A block number
 */
typedef unsigned int blno_t;

/** A data block */
typedef struct Block {
    blno_t no;
    size_t size;
    char *content;
} Block;

#endif /* _COOLFS_BLOCK_H_ */