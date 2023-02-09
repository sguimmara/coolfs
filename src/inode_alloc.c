#include <stdlib.h>
#include <sys/stat.h>
#include "inode_alloc.h"
#include "inode.h"
#include "bitmap.h"

bitmap *inode_bm;

size_t inode_count;
cool_inode **inodes;

void cl_init_inode_allocator(size_t inode_count) {
    inode_bm = bm_alloc(inode_count);
}

cool_inode* cl_new_inode() {
    size_t ino;
    bm_get(inode_bm, &ino);

    inode_count++;
    if (inodes == NULL) {
        inodes = malloc(inode_count * sizeof(cool_inode*));
    } else {
        inodes = realloc(inodes, inode_count * sizeof(cool_inode*));
    }

    cool_inode *new_inode = cl_new_inode_raw(ino);
    inodes[inode_count - 1] = new_inode;
    
    return new_inode;
}

cool_inode *cl_get_inode(ino_t no) { 
    for (size_t i = 0; i < inode_count; i++) {
        if (inodes[i]->st->st_ino == no) {
            return inodes[i];
        }
    }
    
    return NULL;
}