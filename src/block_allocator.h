#ifndef _COOLFS_BLOCK_ALLOCATOR_H_
#define _COOLFS_BLOCK_ALLOCATOR_H_

#include "block.h"

typedef struct DiskUsage {
    size_t block_size;
    size_t allocated_blocks;
    size_t total_capacity;
    size_t used_space;
} DiskUsage;

void block_allocator_init(size_t block_size, size_t capacity);
void block_allocator_destroy();

Block *new_block();
Block *get_block(blno_t no);
void release_block(blno_t no);

blno_t *allocate_blocks(const size_t bufsize, size_t *block_count);

void write_into_blocks(const char *buf, size_t bufsize, blno_t *blocks,
                       size_t block_count, size_t offset);

void read_from_blocks(char *dst, size_t bufsize, blno_t *blocks,
                      size_t block_count);

DiskUsage get_stats();

#endif /* _COOLFS_BLOCK_ALLOCATOR_H_ */
