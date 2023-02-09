#define _XOPEN_SOURCE 1

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "bitmap.h"
#include "block_allocator.h"
#include "constants.h"
#include "dir.h"
#include "fs.h"
#include "inode.h"
#include "inode_alloc.h"

#include "log/log.h"

static char *PATH_SEP = "/";

char MAGIC[6] = {'C', 'O', 'O', 'L', 'F', 'S'};

int cl_mkfs(const char *filename) {
    FILE *file = fopen(filename, "r+");

    if (file == NULL) {
        log_error("%s: could not open file in read mode.", filename);
        return 1;
    }

    struct stat st;
    stat(filename, &st);

    size_t remaining = st.st_size;

    log_info("%s: %lu bytes available", filename, st.st_size);

    log_info("mkfs: writing magic number");
    fwrite(MAGIC, sizeof(char), 6, file);
    remaining -= 6;

    log_info("mkfs: writing block size (%u)", BLOCK_SIZE);
    const u_int16_t bs = (u_int16_t)BLOCK_SIZE;
    fwrite(&bs, sizeof(bs), 1, file);
    remaining -= sizeof(bs);

    log_info("mkfs: writing inode bitmap (%u inodes)", MAX_INODES);
    bitmap *inode_bm = bm_alloc(MAX_INODES);
    fwrite(&inode_bm->bits, sizeof(size_t), 1, file);
    fwrite(&inode_bm->size, sizeof(size_t), 1, file);
    remaining -= (2 * sizeof(size_t));
    fwrite(inode_bm->buf, 1, inode_bm->size, file);
    remaining -= inode_bm->size;
    free(inode_bm);

    size_t inode_area = 1024 * 1024;
    if (remaining < inode_area) {
        log_error("no enough space left");
        fclose(file);
        return 1;
    }
    log_info("mkfs: reserving area for inodes (%u bytes)", inode_area);

    char *reserved = calloc(inode_area, 1);
    fwrite(reserved, inode_area, 1, file);
    free(reserved);
    remaining -= inode_area;

    size_t block_count = remaining / BLOCK_SIZE;
    if (block_count < MIN_BLOCKS) {
        log_error("not enough space to allocate at least %u blocks",
                  MIN_BLOCKS);
        fclose(file);
        return 1;
    }

    log_info("mkfs: writing block bitmap (%u blocks)", block_count);
    bitmap *blk_bm = bm_alloc(block_count);
    fwrite(&blk_bm->bits, sizeof(size_t), 1, file);
    fwrite(&blk_bm->size, sizeof(size_t), 1, file);
    remaining -= (2 * sizeof(size_t));
    fwrite(blk_bm->buf, 1, blk_bm->size, file);
    remaining -= blk_bm->size;
    free(blk_bm);

    // Compute the block count
    size_t block_area = block_count * BLOCK_SIZE;
    if (remaining < block_area) {
        log_error("no enough space left for blocks");
        fclose(file);
        return 1;
    }
    log_info("mkfs: block area: %lu", block_count * BLOCK_SIZE);
    remaining -= block_count * BLOCK_SIZE;

    log_info("unused: %u bytes", remaining);
    log_info("successfully created disk file");
    fclose(file);
    return 0;
}

cool_dirent *cl_find_dirent(const char *path, cool_dirent *root) {
    assert(root != NULL);
    assert(path != NULL);

    log_debug("[resolve] %s", path);

    if (strcmp(path, PATH_SEP) == 0) {
        log_trace("%s -> root", path);
        return root;
    }

    cool_dirent *cur = root;
    char *cpy = malloc(strlen(path) + 1);
    strcpy(cpy, path);

    char *fragment = strtok(cpy, PATH_SEP);

    while (fragment) {
        cur = cl_get_dirent(cur, fragment);
        if (cur == NULL) {
            free(cpy);
            return NULL;
        }

        fragment = strtok(NULL, PATH_SEP);
        if (fragment == NULL) {
            free(cpy);
            log_debug("[resolve] %s -> inode %s", path, cur->name);
            return cur;
        }
    }

    free(cpy);
    return NULL;
}

cool_dirent *root;

void cl_fsinit(cool_dirent *rootent) { root = rootent; }

int cl_open_dev(const char *filename) {
    FILE *file = fopen(filename, "r+");
    if (file == NULL) {
        log_error("could not open %s", filename);
        return 1;
    }

    log_trace("checking magic number");

    for (size_t i = 0; i < 6; i++) {
        if ((char)fgetc(file) != MAGIC[i]) {
            log_error("invalid magic number at offset %u", i);
            return 1;
        }
    }

    u_int16_t block_size;
    fread(&block_size, sizeof(uint16_t), 1, file);
    log_info("block size: %hu", block_size);

    // inode bitmap
    size_t inode_bm_bits;
    size_t inode_bm_bytes;
    fread(&inode_bm_bits, sizeof(size_t), 1, file);
    fread(&inode_bm_bytes, sizeof(size_t), 1, file);
    bitmap *inode_bm = bm_alloc(inode_bm_bits);
    fread(inode_bm->buf, inode_bm_bytes, 1, file);
    log_info("inode bitmap size: %lu", inode_bm_bits);

    return 0;
}

int cl_read(const char *path, char *buf, size_t size, off_t offset,
            struct fuse_file_info *fi) {
    log_info("[read] %s (bufsize: %zo)", path, size);

    cool_dirent *ent = cl_find_dirent(path, root);

    if (ent != NULL) {
        if (ent->type == S_IFREG) {
            cool_inode *inode = cl_get_inode(ent->node.file->inode);
            size_t filesize = inode->st->st_size;
            log_info("[read] copying content of %lo bytes", filesize);
            cl_read_storage(buf, filesize, inode->first_blocks);
            return filesize;
        }
        log_error("[read] not a regular file: %s", path);
        return -1;
    }

    log_error("[read] no such file: %s", path);

    return -1;
}

int cl_open(const char *path, struct fuse_file_info *fi) {
    log_info("[open] %s", path);
    return 0;
}

int cl_getattr(const char *path, struct stat *st) {
    memset(st, 0, sizeof(struct stat));

    cool_dirent *ent = cl_find_dirent(path, root);

    if (ent == NULL) {
        log_trace("[getattr] %s -> ENOENT", path);
        return -ENOENT;
    }

    if (ent->type == S_IFREG) {
        cool_inode *inode = cl_get_inode(ent->node.file->inode);
        memcpy(st, inode->st, sizeof(struct stat));
        log_info("[getattr] %s", path);
        return 0;
    }
    if (ent->type == S_IFDIR) {
        memcpy(st, ent->node.dir->stat, sizeof(struct stat));
        log_info("[getattr] %s", path);
        return 0;
    }

    log_trace("[getattr] %s -> ENOENT", path);
    return -ENOENT;
}

int cl_access(const char *path, int mode) {
    return 0;
}

int cl_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
               off_t offset, struct fuse_file_info *fi) {
    (void)offset;
    (void)fi;

    cool_dirent *ent = cl_find_dirent(path, root);

    if (ent == NULL) {
        log_trace("[readdir] %s -> ENOENT", path);
        return -ENOENT;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (size_t i = 0; i < ent->node.dir->entry_cnt; i++) {
        cool_dirent *child = ent->node.dir->entries[i];
        if (child != NULL) {
            filler(buf, child->name, cl_get_stat(child), 0);
        } else {
            log_warn("[readdir] NULL child");
        }
    }

    log_info("[readdir] %s", path);
    return 0;
}

int cl_chmod(const char *path, mode_t mode) {
    log_trace("[chmod] %s %o", path, mode);
    cool_dirent *ent = cl_find_dirent(path, root);

    if (ent == NULL) {
        log_error("[chmod] %s -> ENOENT", path);
        return -ENOENT;
    }

    switch (ent->type) {
        case S_IFREG:
            cl_get_inode(ent->node.file->inode)->st->st_mode = mode;
            break;
        case S_IFDIR:
            ent->node.dir->stat->st_mode = mode;
            break;
    }

    log_info("[chmod] %s -> %o", path, mode);

    return 0;
}

int cl_write(const char *path, const char *buf, size_t size,
             off_t offset, struct fuse_file_info *fi) {

    cool_dirent *ent = cl_find_dirent(path, root);

    if (ent == NULL) {
        log_error("[write] %s -> ENOENT", path);
        return -ENOENT;
    }

    if (ent->type != S_IFREG) {
        log_error("[write] %s -> not a regular file", path);
        return -ENOENT;
    }

    cool_inode *inode = cl_get_inode(ent->node.file->inode);
    cl_write_storage(buf, size, inode);

    return size;
}
