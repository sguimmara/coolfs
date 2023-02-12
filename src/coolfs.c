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

    ADD_DIR(ROOT, "bin")
    ADD_DIR(ROOT, "usr")
    ADD_DIR(ROOT, "etc")
    ADD_DIR(ROOT, "boot")
    ADD_DIR(ROOT, "root")
    ADD_DIR(ROOT, "home")

    // Dirent *fs_root = cl_new_root();
    // cool_dir *root = fs_root->node.dir;

    // cl_init_inode_allocator(MAX_INODES);

    // cl_add_dir(root, "boot", cl_new_dir());
    // cl_add_dir(root, "root", cl_new_dir());
    // cl_add_dir(root, "bin", cl_new_dir());
    // cl_add_dir(root, "etc", cl_new_dir());
    // Dirent *home = cl_add_dir(root, "home", cl_new_dir());
    // Dirent *jay = cl_add_dir(home->node.dir, "jay", cl_new_dir());
    // cl_add_dir(jay->node.dir, ".config", cl_new_dir());

    // cool_inode *bashrc = cl_new_inode();
    // cl_add_file(jay->node.dir, ".bashrc", cl_new_file(bashrc->st->st_ino));
    // cl_add_dir(root, "var", cl_new_dir());

    // const char bashrc_content[] =
    //         "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do\n"
    //         "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut\n"
    //         "enim ad minim veniam, quis nostrud exercitation ullamco laboris\n"
    //         "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in\n"
    //         "reprehenderit in voluptate velit esse cillum dolore eu fugiat\n"
    //         "nulla pariatur. Excepteur sint occaecat cupidatat non proident,\n"
    //         "sunt in culpa qui officia deserunt mollit anim id est laborum.\n";

    // cl_write_storage(bashrc_content, sizeof(bashrc_content) - 1, bashrc);

    // // cl_print_root(root);

    // cl_fsinit(fs_root);

    log_info("initialized.");

    return NULL;
}

static const struct fuse_operations operations = {
    .init = init,

    .getattr = fuse_getattr,
    .access = fuse_access,

    .readdir = fuse_readdir,

    .open = fuse_open,
    // .read = fuse_read,
    // .write = fuse_write,

    .chmod = fuse_chmod,
    .chown = fuse_chown,
    .unlink = fuse_unlink,
    .rmdir = fuse_rmdir,
    .create = fuse_create,
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

    if (options.verbose) {
        log_set_level(LOG_DEBUG);
    } else {
        log_set_level(LOG_INFO);
    }

    if (options.set_trace) {
        log_set_level(LOG_DEBUG);
    }

    // FILE *storage = fopen("cool.disk", "wb");
    // // cl_mkfs(storage);
    // cl_init_storage(storage);

    ret = fuse_main(args.argc, args.argv, &operations, NULL);

    fuse_opt_free_args(&args);

    // TODO dispose FS
    // cl_storage_dispose();
    // fclose(storage);
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