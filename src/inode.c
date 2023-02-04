#define _XOPEN_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "inode.h"

child_list *mk_default_list() {
  child_list *res = malloc(sizeof(child_list));
  res->elems = NULL;
  res->count = 0;
  return res;
}

cool_inode *mk_inode(ino_t n, char *name, char *data) {
  struct cool_inode *result = malloc(sizeof(cool_inode));

  printf("%s: %s\n", __FUNCTION__, name);

  struct stat *st = malloc(sizeof(struct stat));
  st->st_uid = 1000;
  st->st_gid = 1000;
  st->st_ino = n;
  st->st_mode = S_IFREG | 0444;
  st->st_size = strlen(data);
  st->st_ctime = 1904194;
  st->st_atime = 1904195;
  st->st_mtime = 1904195;

  result->st = st;
  result->data = data;
  result->name = name;
  result->children = mk_default_list();

  return result;
}

cool_inode *get_child(cool_inode *parent, const char *name) {
  for (size_t i = 0; i < parent->children->count; i++) {
    cool_inode *child = parent->children->elems[i];
    if (strcmp(child->name, name) == 0) {
      return child;
    }
  }

  return NULL;
}

cool_inode *mk_root() {
  cool_inode *res = malloc(sizeof(cool_inode));

  res->name = "/";
  res->data = NULL;
  res->children = mk_default_list();

  struct stat *st = malloc(sizeof(struct stat));

  st->st_ino = 1;
  st->st_mode = S_IFDIR | 0755;
  st->st_nlink = 1;

  st->st_uid = 1000;
  st->st_gid = 1000;

  st->st_ctime = 1904194;
  st->st_atime = 1904195;
  st->st_mtime = 1904195;

  res->st = st;

  return res;
}

int add_child(cool_inode *parent, cool_inode *child) {
  size_t new_count = parent->children->count + 1;
  size_t new_size = new_count * sizeof(cool_inode *);

  if (parent->children->elems == NULL) {
    parent->children->elems = malloc(new_size);
  } else {
    parent->children->elems = realloc(parent->children->elems, new_size);
  }

  parent->children->elems[new_count - 1] = child;
  parent->children->count = new_count;

  return 0;
}

void print_tree(cool_inode *inode, int level) {
  char *indent = malloc(level + 1);
  for (size_t i = 0; i < level; i++) {
    indent[i] = ' ';
  }

  indent[level] = '\0';

  printf("%s%s\n", indent, inode->name);

  for (size_t i = 0; i < inode->children->count; i++) {
    cool_inode *child = inode->children->elems[i];
    print_tree(child, level + 1);
  }
}
