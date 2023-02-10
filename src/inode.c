#define _XOPEN_SOURCE 700

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "log/log.h"

#include "inode.h"

void cl_free_inode(cool_inode *inode) {
    free(inode->st);
}

/** Creates an inode with the provided node number */
cool_inode *cl_new_inode_raw(ino_t n) {
    struct cool_inode *result = malloc(sizeof(cool_inode));

    struct stat *st = calloc(1, sizeof(struct stat));
    st->st_uid = 1000;
    st->st_gid = 1000;
    st->st_ino = n;
    st->st_nlink = 1;
    st->st_mode = S_IFREG | 0444;
    st->st_size = 0;
    time_t t = time(NULL);
    st->st_ctime = t;
    st->st_atime = t;
    st->st_mtime = t;

    for (size_t i = 0; i < 10; i++) {
        result->first_blocks[i] = SIZE_MAX;
    }

    result->st = st;

    return result;
}
