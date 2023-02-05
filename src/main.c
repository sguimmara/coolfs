#define FUSE_USE_VERSION 26

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "coolfs.h"
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
    OPTION("--name=%s", filename),
    OPTION("--contents=%s", contents),
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    FUSE_OPT_END
};

static void show_help(const char *progname) {
  printf("usage: %s [options] <mountpoint>\n\n", progname);
}

void *cool_init(struct fuse_conn_info *conn) {
  (void)conn;

  // Don't buffer stdout
  setbuf(stdout, NULL);

  log_info("initializing coolfs...");

  cool_inode *root = mk_root();

  add_child(root, mk_inode(2, "foo", "I am foo"));
  add_child(root, mk_inode(3, "bar", "I am bar"));
  add_child(root, mk_inode(4, "baz", "I am baz"));

  print_tree(root, 0);

  log_info("initialized.");

  return NULL;
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
