#define FUSE_USE_VERSION 29

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>

static struct options {
    const char *filename;
    const char *contents;
    int show_help;
} options;

#define OPTION(t, p) \
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

static void *init(struct fuse_conn_info *conn) {
    (void) conn;
    return NULL;
}

int _read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    for (size_t i = 0; i < size; i++)
    {
        buf[i] = 'a';
    }

    return size;
}

int _open(const char *path, struct fuse_file_info *fi) {
    fi->fh = 10;
    return 10;
}

static const struct fuse_operations operations = {
    .init       = init,
    // .getattr    = getattr,
    // .readdir    = readdir,
    .open       = _open,
    .read       = _read,
};

int main(int argc, char *argv[]) {
    int ret;

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    // options.filename = (const char*) strdup("hello");
    // options.contents = (const char*) strdup("Hello, world!\n");

    if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
        return 1;

    if (options.show_help) {
        show_help(argv[0]);
        assert(fuse_opt_add_arg(&args, "--help") == 0);
        args.argv[0][0] = '\0';
    }

    ret = fuse_main(args.argc, args.argv, &operations, NULL);
    fuse_opt_free_args(&args);

    return ret;
}