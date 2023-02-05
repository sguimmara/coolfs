#ifndef _COOL_INODE_H_
#define _COOL_INODE_H_

#include <stddef.h>
#include <fuse.h>

struct cool_inode;

typedef struct child_list {
  struct cool_inode **elems;
  size_t count;
} child_list;

typedef struct cool_inode {
  struct stat *st;
  char *name;
  char *data;

  struct child_list *children;
} cool_inode;

/** Creates the root node (/) */
cool_inode *mk_root();

cool_inode *mk_inode(ino_t n, char *name, char *data);

child_list *mk_empty_list();

cool_inode *get_child(cool_inode *parent, const char *name);

void print_tree(cool_inode *inode, int level);

int add_child(cool_inode *parent, cool_inode *child);

#endif /* _COOL_INODE_H_ */
