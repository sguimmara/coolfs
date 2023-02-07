#ifndef _COOL_INODE_ALLOC_H_
#define _COOL_INODE_ALLOC_H_

#include <sys/types.h>
#include "inode.h"

void cl_init_inode_allocator(size_t inode_count);

cool_inode *cl_new_inode();

cool_inode *cl_get_inode(ino_t no);

#endif /* _COOL_INODE_ALLOC_H_ */