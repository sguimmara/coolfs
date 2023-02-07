#ifndef _COOL_INODE_H_
#define _COOL_INODE_H_

#include <stddef.h>
#include <sys/types.h>

#include "storage.h"

struct cool_inode;

typedef struct cool_inode {
    struct stat *st;
    size_t block_count;
    blk_no *blocks;
} cool_inode;

void cl_free_inode(cool_inode* inode);

cool_inode *cl_new_inode_raw(ino_t n);

int cl_read_inode(char *buf, cool_inode *node, size_t size, off_t offset);

#endif /* _COOL_INODE_H_ */
