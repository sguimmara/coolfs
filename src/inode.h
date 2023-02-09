#ifndef _COOL_INODE_H_
#define _COOL_INODE_H_

#include <stddef.h>
#include <sys/types.h>

struct cool_inode;

typedef struct cool_inode {
    struct stat *st;
    size_t first_blocks[10];
} cool_inode;

void cl_free_inode(cool_inode* inode);

cool_inode *cl_new_inode_raw(ino_t n);

#endif /* _COOL_INODE_H_ */
