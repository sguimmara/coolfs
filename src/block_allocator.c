#include <errno.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>

#include "block_allocator.h"
#include "bitmap.h"
#include "block.h"

size_t BLOCK_SIZE;     // In bytes
size_t TOTAL_CAPACITY; // How many blocks can we store in the block array
size_t current_capacity; // The current cap of the block array
bitmap *block_usage; // Bitmap containing the used/non used blocks
size_t block_count; // The number of used blocks
Block **blocks;  // The block array

void block_allocator_init(size_t blck_size, size_t cap) {
    block_usage = bm_alloc(cap);
    BLOCK_SIZE = blck_size;
    TOTAL_CAPACITY = cap;
    current_capacity = 0;
    block_count = 0;
    blocks = NULL;
}

void expand_block_array() {
    if (current_capacity == 0) {
        current_capacity = 1024;
    } else {
        current_capacity *= 2;
    }
    if (current_capacity > TOTAL_CAPACITY) {
        current_capacity = TOTAL_CAPACITY;
    }
    blocks = realloc(blocks, current_capacity * sizeof(Block *));
}

void add_block(Block *block) {
    // sanity check
    assert(bm_is_set(block_usage, (size_t)block->no) == true);

    if (current_capacity == 0 || block->no > (current_capacity - 1)) {
        expand_block_array();
    }

    assert(blocks != NULL);

    blocks[block->no] = block;

    block_count++;
}

Block *new_block() {
    size_t blno;
    if (bm_get(block_usage, &blno) != 0) {
        errno = ENOSPC;
        return NULL;
    }

    Block *result = malloc(sizeof(Block));
    result->no = (blno_t)blno;
    result->size = 0;
    result->flags = BLCK_CHNG;
    result->content = malloc(BLOCK_SIZE);

    add_block(result);

    return result;
}

Block *get_block(blno_t no) {
    assert(no < current_capacity);
    assert(bm_is_set(block_usage, (size_t)no));

    return blocks[(size_t)no];
}

void free_block(Block *block) { free(block->content); }

void release_block(blno_t no) {
    assert(bm_is_set(block_usage, (size_t)no));

    Block *block = get_block(no);
    bm_unset(block_usage, (size_t)no);
    free_block(block);
    blocks[no] = NULL;
    block_count--;
}

/**
 * @brief How many blocks to store BUFSIZE ?
 */
size_t count_blocks(size_t bufsize) {
    return (bufsize / BLOCK_SIZE) + (bufsize % BLOCK_SIZE > 0 ? 1 : 0);
}

blno_t *allocate_blocks(size_t bufsize, size_t *blck_count) {
    *blck_count = count_blocks(bufsize);

    blno_t *res = malloc(*blck_count * sizeof(blno_t));

    for (size_t i = 0; i < *blck_count; i++) {
        Block *b = new_block();
        if (b == NULL) {
            free(res);
            return NULL;
        }
        res[i] = b->no;
    }

    return res;
}

void write_into_blocks(const char *buf, size_t bufsize, blno_t *blocks,
                       size_t block_count, off_t offset) {
    size_t remaining = bufsize;
    char *ptr = (char *)buf;

    for (size_t i = 0; i < bufsize; i += BLOCK_SIZE) {
        size_t block_index = (i + offset) / BLOCK_SIZE;
        size_t block_offset = offset % BLOCK_SIZE;
        blno_t blno = blocks[block_index];
        Block *block = get_block(blno);
        size_t size = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
        block->size = size;
        memcpy(block->content + block_offset, ptr + i, size);
        block->flags = BLCK_CHNG;
        remaining -= BLOCK_SIZE;
    }
}

void read_from_blocks(char *dst, size_t bufsize, blno_t *blocks,
                      size_t block_count, off_t offset) {
    size_t dst_offset = 0;
    size_t first_block = (offset / BLOCK_SIZE);
    for (size_t i = first_block; i < block_count; i++) {
        Block *block = get_block(blocks[i]);
        off_t block_offset = i == first_block ? (offset % BLOCK_SIZE) : 0;
        memcpy(dst + dst_offset, block->content + block_offset, block->size);
        dst_offset += (block->size - block_offset);
    }
}

DiskUsage get_stats() {
    DiskUsage result = {.allocated_blocks = block_count,
                        .block_size = BLOCK_SIZE,
                        .total_capacity = TOTAL_CAPACITY * BLOCK_SIZE,
                        .used_space = block_count * BLOCK_SIZE};

    return result;
}

void block_allocator_destroy() {
    for (size_t i = 0; i < current_capacity; i++) {
        if (bm_is_set(block_usage, i)) {
            free_block(blocks[i]);
        }
    }

    free(blocks);

    bm_free(block_usage);
}
