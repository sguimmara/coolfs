#include "config.h"

#ifdef HAVE_LIBFUSE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <fuse.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "log/log.h"

#include "fuseimpl.h"
#include "inode.h"

#define ROOT get_root()

#define ADD_DIR(parent, name) \
    { \
        add_entry(parent, name, create_directory(parent)->number); \
    }

static struct options {
    const char *filename;
    const char *contents;
    int show_help;
    int verbose;
    int set_trace;
} options;

#define OPTION(t, p)                                                           \
    { t, offsetof(struct options, p), 1 }

static const struct fuse_opt option_spec[] = {
    OPTION("-h", show_help),
    OPTION("--help", show_help),
    OPTION("--trace", set_trace),
    OPTION("-v", verbose),
    FUSE_OPT_END
};

static void show_help(const char *progname) {
    printf("usage: %s [options] <mountpoint>\n\n", progname);
}

void *init(struct fuse_conn_info *conn) {
    (void)conn;

    fuse_init();

    log_info("initialized.");

    return NULL;
}

static const struct fuse_operations operations = {
    .init = init,

    .getattr = fuse_getattr,
    .access = fuse_access,

    .readdir = fuse_readdir,

    .open = fuse_open,
    .read = fuse_read,
    .write = fuse_write,

    .chmod = fuse_chmod,
    .chown = fuse_chown,
    .unlink = fuse_unlink,
    .rmdir = fuse_rmdir,
    .create = fuse_create,

    .rename = fuse_rename,
    .mkdir = fuse_mkdir,

    .destroy = fuse_deinit,
};

int main(int argc, char *argv[]) {
    int ret;

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // Don't buffer stdout
    setbuf(stdout, NULL);
    log_set_level(0);

    log_info("%s", PACKAGE_STRING);

    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return 1;

    if (options.show_help) {
        show_help(argv[0]);
        assert(fuse_opt_add_arg(&args, "--help") == 0);
        args.argv[0][0] = '\0';
    }

    if (options.set_trace) {
        log_set_level(LOG_DEBUG);
    } else if (options.verbose) {
        log_set_level(LOG_DEBUG);
    } else {
        log_set_level(LOG_INFO);
    }

    ret = fuse_main(args.argc, args.argv, &operations, NULL);

    fuse_opt_free_args(&args);

    log_info("exiting");

    return ret;
}

#else

#include <stdio.h>

int main(void) {
    fprintf(stderr, "FUSE is not available in this system. Exiting.\n");
    return 1;
}

#endif /* HAVE_LIBFUSE */