#define _XOPEN_SOURCE 700

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "log/log.h"

#include "inode.h"
#include "storage.h"

void free_inode(cool_inode *inode) {
    free(inode->st);
    free(inode->blocks);
}

/** Creates an inode with the provided node number */
cool_inode *mk_inode(ino_t n) {
    struct cool_inode *result = malloc(sizeof(cool_inode));

    struct stat *st = calloc(1, sizeof(struct stat));
    st->st_uid = 1000;
    st->st_gid = 1000;
    st->st_ino = n;
    st->st_mode = S_IFREG | 0444;
    st->st_size = 0;
    time_t t = time(NULL);
    st->st_ctime = t;
    st->st_atime = t;
    st->st_mtime = t;

    result->st = st;

    return result;
}

int inode_read(char *buf, cool_inode *node, size_t size, off_t offset) {
    log_trace("[inode_read] %lo", node->block_count);

    assert(node != NULL);
    assert(node->block_count > 0);
    assert(size <= node->st->st_size);

    storage_read(buf, size, node->blocks, node->block_count);

    return 0;
}
