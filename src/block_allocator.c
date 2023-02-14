#include "block_allocator.h"
#include "bitmap.h"
#include "block.h"
#include "errno.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

size_t block_size;
size_t total_capacity;
size_t current_capacity;
bitmap *block_usage;
size_t block_count;
Block **blocks;

void block_allocator_init(size_t blck_size, size_t cap) {
    block_usage = bm_alloc(cap);
    block_size = blck_size;
    total_capacity = cap;
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
    if (current_capacity > total_capacity) {
        current_capacity = total_capacity;
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
    result->content = malloc(block_size);

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
    return (bufsize / block_size) + (bufsize % block_size > 0 ? 1 : 0);
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
                       size_t block_count) {
    off_t offset = 0;
    size_t current_blck = 0;
    size_t remaining = bufsize;
    char *ptr = (char *)buf;

    for (size_t i = 0; i < bufsize; i += block_size) {
        blno_t blno = blocks[current_blck++];
        Block *block = get_block(blno);
        size_t size = remaining < block_size ? remaining : block_size;
        block->size = size;
        memcpy(block->content, ptr + i, size);
        remaining -= block_size;
    }
}

void read_from_blocks(char *dst, size_t bufsize, blno_t *blocks,
                      size_t block_count) {
    size_t offset = 0;
    for (size_t i = 0; i < block_count; i++) {
        Block *block = get_block(blocks[i]);
        memcpy(dst + offset, block->content, block->size);
        offset += block->size;
    }
}

DiskUsage get_stats() {
    DiskUsage result = {.allocated_blocks = block_count,
                        .block_size = block_size,
                        .total_capacity = total_capacity * block_size,
                        .used_space = block_count * block_size};

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
