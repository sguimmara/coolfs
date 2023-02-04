#define FUSE_USE_VERSION 29
#define _XOPEN_SOURCE 1

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

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

typedef struct cool_inode {
    struct stat* st;
    char* name;
    char* data;
} cool_inode;

static cool_inode* mk_inode(ino_t n, char* name, char* data) {
    struct cool_inode *result = malloc(sizeof(cool_inode));

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

    return result;
}

const int FILE_COUNT = 3;

cool_inode *FILES[3];

void *cool_init(struct fuse_conn_info *conn) {
    (void) conn;

    // Don't buffer stdout
    setbuf(stdout, NULL);

    puts("initializing coolfs");

    // size_t ptrsize = sizeof(cool_inode*);

    // FILES = malloc(ptrsize * FILE_COUNT);

    FILES[0] = mk_inode(2, "foo", "I am foo");
    FILES[1] = mk_inode(3, "bar", "I am bar");
    FILES[2] = mk_inode(4, "baz", "I am baz");

    puts("files created");

    return NULL;
}

cool_inode* 
cool_find_inode(const char *path) {
    printf("\n%s: %s\n", __FUNCTION__, path);

    for (size_t i = 0; i < FILE_COUNT; i++)
    {
        cool_inode *inode = FILES[i];
        if (strcmp(path + 1, inode->name) == 0) {
            printf("%s: %s -> found\n", __FUNCTION__, path);
            return inode;
        }
    }

    return NULL;
}

int cool_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("\n%s: %s (bufsize: %zo)\n", __FUNCTION__, path, size);

    cool_inode *inode = cool_find_inode(path);
    
    if (inode != NULL) {
        off_t size = inode->st->st_size;
        printf("%s: copying content of %lo bytes\n", __FUNCTION__, size);
        strcpy(buf, inode->data);
        return size;
    }

    return -1;
}

int cool_open(const char *path, struct fuse_file_info *fi) {
    printf("\n%s: %s\n", __FUNCTION__, path);
    fi->fh = 10;
    return 0;
}

int cool_getattr(const char *path, struct stat *st) {
    memset(st, 0, sizeof(struct stat));

    printf("\n%s: %s\n", __FUNCTION__, path);

    if (strcmp(path, "/") == 0) {
        st->st_ino = 1;
        st->st_mode = S_IFDIR | 0444;
        st->st_nlink = 1;

        st->st_uid = 1000;
        st->st_gid = 1000;

        st->st_ctime = 1904194;
        st->st_atime = 1904195;
        st->st_mtime = 1904195;
        printf("%s: %s -> returned root node\n", __FUNCTION__, path);
        return 0;
    }


    for (size_t i = 0; i < FILE_COUNT; i++)
    {
        cool_inode *inode = FILES[i];
        if (strcmp(path + 1, inode->name) == 0) {
            memcpy(st, inode->st, sizeof(struct stat));
            printf("%s: %s -> file found\n", __FUNCTION__, path);
            return 0;
        }
    }

    printf("%s: %s -> ENOENT\n", __FUNCTION__, path);

    return -ENOENT;
}

int cool_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi) {
    (void) offset;
    (void) fi;

    printf("\n%s: %s\n", __FUNCTION__, path);

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (size_t i = 0; i < FILE_COUNT; i++)
    {
        cool_inode *inode = FILES[i];
        filler(buf, inode->name, inode->st, 0);
        printf("%s: %s\n", __FUNCTION__, inode->name);
    }

    return 0;
}

static const struct fuse_operations operations = {
    .init       = cool_init,
    .getattr    = cool_getattr,
    .readdir    = cool_readdir,
    .open       = cool_open,
    .read       = cool_read,
};

int main(int argc, char *argv[]) {
    int ret;

    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    puts("starting coolfs...");

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