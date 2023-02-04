#define FUSE_USE_VERSION 29
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

#include "inode.h"

#include "log/log.h"

static struct options {
  const char *filename;
  const char *contents;
  int show_help;
} options;

#define OPTION(t, p)                                                           \
  { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
    OPTION("--name=%s", filename), OPTION("--contents=%s", contents),
    OPTION("-h", show_help), OPTION("--help", show_help), FUSE_OPT_END};

static void show_help(const char *progname) {
  printf("usage: %s [options] <mountpoint>\n\n", progname);
}

const int FILE_COUNT = 3;

cool_inode *root;

void *cool_init(struct fuse_conn_info *conn) {
  (void)conn;

  // Don't buffer stdout
  setbuf(stdout, NULL);

  log_info("initializing coolfs...");

  root = mk_root();

  add_child(root, mk_inode(2, "foo", "I am foo"));
  add_child(root, mk_inode(3, "bar", "I am bar"));
  add_child(root, mk_inode(4, "baz", "I am baz"));

  print_tree(root, 0);

  log_info("initialized.");

  return NULL;
}

cool_inode *cool_find_inode(const char *path) {
  log_debug("\n%s: %s\n", __FUNCTION__, path);

  if (strcmp(path, "/") == 0) {
    return root;
  }

  cool_inode *cur = root;
  char *cpy = malloc(strlen(path));
  strcpy(cpy, path);

  char *sep = "/";
  char *fragment = strtok(cpy, sep);

  while (fragment) {
    cur = get_child(cur, fragment);
    if (cur == NULL) {
      return NULL;
    }

    return cur;
    fragment = strtok(NULL, sep);
    if (fragment == NULL) {
      log_debug("%s -> inode %s", path, cur->name);
      return cur;
    }
  }

  return NULL;
}

int cool_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi) {
  log_debug("\n%s: %s (bufsize: %zo)\n", __FUNCTION__, path, size);

  cool_inode *inode = cool_find_inode(path);

  if (inode != NULL) {
    off_t size = inode->st->st_size;
    log_trace("%s: copying content of %lo bytes\n", __FUNCTION__, size);
    strcpy(buf, inode->data);
    return size;
  }

  return -1;
}

int cool_open(const char *path, struct fuse_file_info *fi) {
  log_debug("\n%s: %s\n", __FUNCTION__, path);
  fi->fh = 10;
  return 0;
}

int cool_getattr(const char *path, struct stat *st) {
  log_debug("%s", path);

  memset(st, 0, sizeof(struct stat));

  cool_inode *inode = cool_find_inode(path);

  if (inode != NULL) {
    memcpy(st, inode->st, sizeof(struct stat));
    return 0;
  }

  return -ENOENT;
}

int cool_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
  (void)offset;
  (void)fi;

  cool_inode *inode = cool_find_inode(path);

  if (inode == NULL) {
    return -ENOENT;
  }

  printf("\n%s: %s (%lo children)\n", __FUNCTION__, inode->name,
         inode->children->count);

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  for (size_t i = 0; i < inode->children->count; i++) {
    cool_inode *child = inode->children->elems[i];
    filler(buf, child->name, child->st, 0);
  }

  return 0;
}

static const struct fuse_operations operations = {
    .init = cool_init,

    .getattr = cool_getattr,

    .readdir = cool_readdir,

    .open = cool_open,
    .read = cool_read,
};

int main(int argc, char *argv[]) {
  int ret;

  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  log_set_level(0);
  log_info("starting coolfs...");

  if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
    return 1;

  if (options.show_help) {
    show_help(argv[0]);
    assert(fuse_opt_add_arg(&args, "--help") == 0);
    args.argv[0][0] = '\0';
  }

  ret = fuse_main(args.argc, args.argv, &operations, NULL);

  fuse_opt_free_args(&args);

  log_info("exiting");

  return ret;
}
