#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/log.h"

#include "bitmap.h"
#include "block_allocator.h"
#include "constants.h"
#include "inode.h"

static block **blocks;
static size_t allocated_blocks;
static int should_sort;
bitmap *blk_bitmap;
FILE *disk;
size_t total_storage;
size_t used_storage;

block *cl_new_block(const char *data, const size_t size) {
    if (size > BLOCK_SIZE) {
        return NULL;
    }

    size_t no;
    if (bm_get(blk_bitmap, &no) != 0) {
        log_error("could not allocate block: no empty block free.");
        return NULL;
    }

    block *blk = malloc(sizeof(block));
    blk->no = no;
    blk->size = size;

    log_info("[block] creating block %lu", blk->no);

    char *content = malloc(size);
    memcpy(content, data, size);
    blk->content = content;

    used_storage += BLOCK_SIZE;

    log_trace("usage: %u / %u B", used_storage, total_storage);

    return blk;
}

void cl_storage_dispose() {
    for (size_t i = 0; i < allocated_blocks; i++) {
        free(blocks[i]->content);
        free(blocks[i]);
    }
}

void cl_init_storage(FILE *file) {
    disk = file;
    allocated_blocks = 0;
    log_trace("cl_init_storage: allocating %u blocks", MAX_BLOCKS);
    blk_bitmap = bm_alloc(1024);
    total_storage = BLOCK_SIZE * MAX_BLOCKS;
    blocks = NULL;
    should_sort = 0;
}

static int cl_compare_blocks(const void *a, const void *b) {
    const block *ba = (const block *)a;
    const block *bb = (const block *)b;
    if (ba->no > bb->no) {
        return 1;
    }

    return -1;
}

void cl_free_block(size_t no) {
    if (no == SIZE_MAX) {
        return;
    }
    bm_unset(blk_bitmap, no);
    free(blocks[no]);
    log_debug("freeing block %lu", no);
}

void cl_sort_blocks() {
    if (allocated_blocks == 0) {
        return;
    }

    qsort(blocks, allocated_blocks, sizeof(block *), cl_compare_blocks);
}

int cl_write_storage(const char *data, const size_t size, cool_inode *inode) {
    size_t count = size / BLOCK_SIZE;

    if (size % BLOCK_SIZE != 0) {
        count++;
    }

    if (count > 10) {
        log_error("too many blocks");
        return -1;
    }

    log_trace("[block] writing %lu bytes into %lu blocks of size %lu",
              size, count, BLOCK_SIZE);

    void *offset = (void *)data;
    size_t remaining = size;

    size_t *result = malloc(sizeof(size_t) * count);

    for (size_t i = 0; i < count; i++) {
        size_t block_size = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        block *blk = cl_new_block(offset, block_size);

        if (blk == NULL) {
            log_error("[block] failed to allocate block.");
            return -1;
        }
        cl_write_block(blk);
        result[i] = blk->no;
        log_trace("[block] writing %u on block %u", block_size, blk->no);
        offset += BLOCK_SIZE;
        remaining -= BLOCK_SIZE;
        inode->first_blocks[i] = blk->no;
    }

    inode->st->st_size = size;
    inode->st->st_blksize = BLOCK_SIZE;
    log_info("[block] total size: %lu bytes", inode->st->st_size);

    memcpy(inode->first_blocks, result, count);

    return 0;
}

int cl_read_storage(char *buf, size_t size, size_t *blks) {
    void *offset = buf;

    size_t block_count = size / BLOCK_SIZE;


    if (size % BLOCK_SIZE != 0) {
        block_count++;
    }
    
    log_trace("[block] %lu blocks to read", block_count);

    for (size_t i = 0; i < block_count; i++) {
        if (blks[i] == SIZE_MAX) {
            log_error("[block] trying to read a non allocated block");
            return -1;
        }
        size_t no = blks[i];
        block *blk = cl_get_block(no);

        if (blk == NULL) {
            log_error("[block] could not get block %u", no);
            return -1;
        }

        memcpy(offset, blk->content, blk->size);
        log_trace("[block] read %lu bytes successfully", size);
        offset += blk->size;
    }

    return 0;
}

void cl_write_block(block *blk) {
    ++allocated_blocks;
    size_t new_size = allocated_blocks * sizeof(block *);
    if (allocated_blocks == 0) {
        blocks = malloc(new_size);
    } else {
        blocks = realloc(blocks, new_size);
    }
    blocks[allocated_blocks - 1] = blk;
    should_sort = 1;
}

block *cl_get_block(size_t no) {
    if (allocated_blocks == 0) {
        log_error("[block] there are zero blocks allocated");
        return NULL;
    }

    if (should_sort) {
        cl_sort_blocks();
    }

    for (size_t i = 0; i < allocated_blocks; i++) {
        block *blk = blocks[i];
        if (blk->no == no) {
            return blk;
        }
    }

    return NULL;
}
