#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/log.h"

#include "storage.h"

static block **blocks;
static size_t block_count;
static int should_sort;

static blno_t current_block_no = 0;

char MAGIC[6] = {'C', 'O', 'O', 'L', 'F', 'S'};

int mkfs(FILE *file) {
    fwrite(MAGIC, sizeof(char), 6, file);
    const u_int16_t bs = BLOCK_SIZE;
    fwrite(&bs, sizeof(u_int16_t), 1, file);

    size_t inode_area_size = sizeof(disk_inode) * MAX_INODES;
    char *inode_area = malloc(inode_area_size);

    fwrite(inode_area, inode_area_size, 1, file);

    return 0;
}

block *make_block(const char *data, const size_t size) {
    if (size > BLOCK_SIZE) {
        return NULL;
    }

    block *blk = malloc(sizeof(block));
    blk->no = current_block_no++;
    blk->size = size;

    char *content = malloc(size);
    memcpy(content, data, size);
    blk->content = content;

    return blk;
}

void dev_init() {
    block_count = 0;
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

blno_t *storage_write(const char *data, const size_t size, size_t *blk_count) {
    *blk_count = size / BLOCK_SIZE;
    if (size % BLOCK_SIZE != 0) {
        (*blk_count)++;
    }

    log_trace("storage_write: writing %lo bytes into %lo blocks of size %o",
              size, *blk_count, BLOCK_SIZE);

    void *offset = (void *)data;
    size_t remaining = size;

    blno_t *result = malloc(sizeof(blno_t) * *blk_count);

    for (size_t i = 0; i < *blk_count; i++) {
        size_t block_size = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        block *blk = make_block(offset, block_size);
        block_write(blk);
        result[i] = blk->no;
        log_trace("writing %lo on block #%lo", block_size, blk->no);
        offset += BLOCK_SIZE;
        remaining -= BLOCK_SIZE;
    }

    return result;
}

int storage_read(char *buf, size_t size, blno_t *blks, size_t blk_count) {
    void *offset = buf;

    for (size_t i = 0; i < blk_count; i++) {
        blno_t no = blks[i];
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

block *get_block(blno_t no) {
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
