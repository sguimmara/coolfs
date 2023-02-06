#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/log.h"

#include "storage.h"
#include "bitmap.h"

#define MAX_BLOCKS 1024

static block **blocks;
static size_t block_count;
static int should_sort;
bitmap *blk_bitmap;
FILE *disk;
size_t total_storage;
size_t used_storage;

char MAGIC[6] = {'C', 'O', 'O', 'L', 'F', 'S'};

int mkfs(FILE *file) {
    log_trace("mkfs: writing magic number");
    fwrite(MAGIC, sizeof(char), 6, file);
    log_trace("mkfs: writing block size (%u)", BLOCK_SIZE);
    const u_int16_t bs = BLOCK_SIZE;
    fwrite(&bs, sizeof(u_int16_t), 1, file);

    size_t inode_area_size = sizeof(disk_inode) * MAX_INODES;
    log_trace("mkfs: allocating inode area (%u bytes)", inode_area_size);
    char *inode_area = calloc(inode_area_size, 1);

    size_t blk_bitmap_size = MAX_BLOCKS / 8;
    log_trace("mkfs: allocating block bitmap (%u bytes)", blk_bitmap_size);
    char *blk_bitmap_area = calloc(inode_area_size, 1);
    fwrite(blk_bitmap_area, blk_bitmap_size, 1, file);

    size_t blk_size = MAX_BLOCKS * BLOCK_SIZE;
    log_trace("mkfs: allocating block area (%u bytes)", blk_size);
    char *blk_area = calloc(inode_area_size, 1);
    fwrite(blk_area, blk_size, 1, file);

    log_trace("writing to disk");

    free(inode_area);
    free(blk_bitmap_area);
    free(blk_area);

    log_info("successfully created disk file");
    return 0;
}

block *make_block(const char *data, const size_t size) {
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

void sto_dispose() {
    for (size_t i = 0; i < block_count; i++) {
        free(blocks[i]->content);
        free(blocks[i]);
    }
}

void sto_init(FILE* file) {
    disk = file;
    block_count = 0;
    log_trace("sto_init: allocating %u blocks", MAX_BLOCKS);
    blk_bitmap = bm_alloc(1024);
    total_storage = BLOCK_SIZE * MAX_BLOCKS;
    blocks = NULL;
    should_sort = 0;
}

static int compare_blks(const void *a, const void *b) {
    const block *ba = (const block *)a;
    const block *bb = (const block *)b;
    if (ba->no > bb->no) {
        return 1;
    }

    return -1;
}

void sort_blocks() {
    if (block_count == 0) {
        return;
    }

    qsort(blocks, block_count, sizeof(block *), compare_blks);
}

blk_no *storage_write(const char *data, const size_t size, size_t *blk_count) {
    *blk_count = size / BLOCK_SIZE;
    if (size % BLOCK_SIZE != 0) {
        (*blk_count)++;
    }

    log_trace("storage_write: writing %lu bytes into %lu blocks of size %lu",
              size, *blk_count, BLOCK_SIZE);

    void *offset = (void *)data;
    size_t remaining = size;

    blk_no *result = malloc(sizeof(blk_no) * *blk_count);

    for (size_t i = 0; i < *blk_count; i++) {
        size_t block_size = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        block *blk = make_block(offset, block_size);

        if (blk == NULL) {
            log_error("storage_write: failed to allocate block.");
            return NULL;
        }
        block_write(blk);
        result[i] = blk->no;
        log_trace("storage_write: writing %u on block %u", block_size, blk->no);
        offset += BLOCK_SIZE;
        remaining -= BLOCK_SIZE;
    }

    return result;
}

int storage_read(char *buf, size_t size, blk_no *blks, size_t blk_count) {
    void *offset = buf;

    for (size_t i = 0; i < blk_count; i++) {
        blk_no no = blks[i];
        block *blk = get_block(no);

        memcpy(offset, blk->content, blk->size);
        offset += blk->size;
    }

    return 0;
}

void block_write(block *blk) {
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

block *get_block(blk_no no) {
    if (block_count == 0) {
        return NULL;
    }

    if (should_sort) {
        sort_blocks();
    }

    for (size_t i = 0; i < block_count; i++) {
        block *blk = blocks[i];
        if (blk->no == no) {
            return blk;
        }
    }

    return NULL;
}
