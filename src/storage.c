#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/log.h"

#include "bitmap.h"
#include "storage.h"
#include "constants.h"

static block **blocks;
static size_t block_count;
static int should_sort;
bitmap *blk_bitmap;
FILE *disk;
size_t total_storage;
size_t used_storage;

block *cl_new_block(const char *data, const size_t size) {
    if (size > BLOCK_SIZE) {
        return NULL;
    }

    blk_no no;
    if (bm_get(blk_bitmap, &no) != 0) {
        log_error("could not allocate block: no empty block free.");
        return NULL;
    }

    block *blk = malloc(sizeof(block));
    blk->no = no;
    blk->size = size;

    char *content = malloc(size);
    memcpy(content, data, size);
    blk->content = content;

    used_storage += BLOCK_SIZE;

    log_trace("usage: %u / %u B", used_storage, total_storage);

    return blk;
}

void cl_storage_dispose() {
    for (size_t i = 0; i < block_count; i++) {
        free(blocks[i]->content);
        free(blocks[i]);
    }
}

void cl_init_storage(FILE *file) {
    disk = file;
    block_count = 0;
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

void cl_sort_blocks() {
    if (block_count == 0) {
        return;
    }

    qsort(blocks, block_count, sizeof(block *), cl_compare_blocks);
}

blk_no *cl_write_storage(const char *data, const size_t size, size_t *blk_count) {
    *blk_count = size / BLOCK_SIZE;
    if (size % BLOCK_SIZE != 0) {
        (*blk_count)++;
    }

    log_trace("cl_write_storage: writing %lu bytes into %lu blocks of size %lu",
              size, *blk_count, BLOCK_SIZE);

    void *offset = (void *)data;
    size_t remaining = size;

    blk_no *result = malloc(sizeof(blk_no) * *blk_count);

    for (size_t i = 0; i < *blk_count; i++) {
        size_t block_size = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        block *blk = cl_new_block(offset, block_size);

        if (blk == NULL) {
            log_error("cl_write_storage: failed to allocate block.");
            return NULL;
        }
        cl_write_block(blk);
        result[i] = blk->no;
        log_trace("cl_write_storage: writing %u on block %u", block_size, blk->no);
        offset += BLOCK_SIZE;
        remaining -= BLOCK_SIZE;
    }

    return result;
}

int cl_read_storage(char *buf, size_t size, blk_no *blks, size_t blk_count) {
    void *offset = buf;

    for (size_t i = 0; i < blk_count; i++) {
        blk_no no = blks[i];
        block *blk = cl_get_block(no);

        memcpy(offset, blk->content, blk->size);
        offset += blk->size;
    }

    return 0;
}

void cl_write_block(block *blk) {
    ++block_count;
    size_t new_size = block_count * sizeof(block *);
    if (block_count == 0) {
        blocks = malloc(new_size);
    } else {
        blocks = realloc(blocks, new_size);
    }
    blocks[block_count - 1] = blk;
    should_sort = 1;
}

block *cl_get_block(blk_no no) {
    if (block_count == 0) {
        return NULL;
    }

    if (should_sort) {
        cl_sort_blocks();
    }

    for (size_t i = 0; i < block_count; i++) {
        block *blk = blocks[i];
        if (blk->no == no) {
            return blk;
        }
    }

    return NULL;
}
