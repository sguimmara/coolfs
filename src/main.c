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

#include "fs.h"
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
    OPTION("-h", show_help), OPTION("--help", show_help), FUSE_OPT_END};

static void show_help(const char *progname) {
    printf("usage: %s [options] <mountpoint>\n\n", progname);
}

void *cool_init(struct fuse_conn_info *conn) {
    (void)conn;

    // Don't buffer stdout
    setbuf(stdout, NULL);

    log_info("initializing coolfs...");

    cool_inode *root = mk_root();

    add_child(
        root,
        mk_inode(
            2, "lorem",
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
            "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
            "enim ad minim veniam, quis nostrud exercitation ullamco laboris "
            "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
            "reprehenderit in voluptate velit esse cillum dolore eu fugiat "
            "nulla pariatur. Excepteur sint occaecat cupidatat non proident, "
            "sunt in culpa qui officia deserunt mollit anim id est laborum."));
    add_child(root, mk_inode(3, "bar", "I am bar"));
    add_child(root, mk_inode(4, "baz", "I am baz"));
    add_child(root, mk_dir(5, "subdir"));

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
