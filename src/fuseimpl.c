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
#include <unistd.h>

#include "log/log.h"

#include "fuseimpl.h"
#include "bitmap.h"
#include "block_allocator.h"
#include "constants.h"
#include "inode.h"
#include "fs.h"
#include "path.h"

void fuse_init() {
    fs_init();
}

int checked(const char *fname, const char *path, int ret) {
    if (ret != 0) {
        int err = errno;
        log_error("%s: %s -> %s (%d)", fname, path, strerror(-ret), ret);
    }

    return ret;
}

int fuse_getattr(const char *path, struct stat *st) {
    return checked(__FUNCTION__, path, _getattr(path, st));
}

int fuse_access(const char *path, int mode) {
    return checked(__FUNCTION__, path, _access(path, mode));
}

int fuse_chmod(const char *path, mode_t mode) {
    return checked(__FUNCTION__, path, _chmod(path, mode));
}

int fuse_chown(const char *path, uid_t uid, gid_t gid) {
    return checked(__FUNCTION__, path, _chown(path, uid, gid));
}

int fuse_unlink(const char *path) {
    return checked(__FUNCTION__, path, _unlink(path));
}

int fuse_rmdir(const char *path) {
    return checked(__FUNCTION__, path, _rmdir(path));
}

int fuse_readdir(const char *path,
                 void *buf,
                 fuse_fill_dir_t filler,
                 off_t offset,
                 struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _readdir(path, buf, filler, offset));
}

int fuse_open(const char *path, struct fuse_file_info *fi) {
    // TODO what should we do here ? see open(2)
    return 0;
}

int fuse_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    return checked(__FUNCTION__, path, _create(path, mode));
}

// static char *PATH_SEP = "/";

// char MAGIC[6] = {'C', 'O', 'O', 'L', 'F', 'S'};

// int cl_mkfs(const char *filename) {
//     FILE *file = fopen(filename, "r+");

//     if (file == NULL) {
//         log_error("%s: could not open file in read mode.", filename);
//         return 1;
//     }

//     struct stat st;
//     stat(filename, &st);

//     size_t remaining = st.st_size;

//     log_debug("%s: %lu bytes available", filename, st.st_size);

//     log_debug("mkfs: writing magic number");
//     fwrite(MAGIC, sizeof(char), 6, file);
//     remaining -= 6;

//     log_debug("mkfs: writing block size (%u)", BLOCK_SIZE);
//     const u_int16_t bs = (u_int16_t)BLOCK_SIZE;
//     fwrite(&bs, sizeof(bs), 1, file);
//     remaining -= sizeof(bs);

//     log_debug("mkfs: writing inode bitmap (%u inodes)", MAX_INODES);
//     bitmap *inode_bm = bm_alloc(MAX_INODES);
//     fwrite(&inode_bm->bits, sizeof(size_t), 1, file);
//     fwrite(&inode_bm->size, sizeof(size_t), 1, file);
//     remaining -= (2 * sizeof(size_t));
//     fwrite(inode_bm->buf, 1, inode_bm->size, file);
//     remaining -= inode_bm->size;
//     free(inode_bm);

//     size_t inode_area = 1024 * 1024;
//     if (remaining < inode_area) {
//         log_error("no enough space left");
//         fclose(file);
//         return 1;
//     }
//     log_debug("mkfs: reserving area for inodes (%u bytes)", inode_area);

//     char *reserved = calloc(inode_area, 1);
//     fwrite(reserved, inode_area, 1, file);
//     free(reserved);
//     remaining -= inode_area;

//     size_t block_count = remaining / BLOCK_SIZE;
//     if (block_count < MIN_BLOCKS) {
//         log_error("not enough space to allocate at least %u blocks",
//                   MIN_BLOCKS);
//         fclose(file);
//         return 1;
//     }

//     log_debug("mkfs: writing block bitmap (%u blocks)", block_count);
//     bitmap *blk_bm = bm_alloc(block_count);
//     fwrite(&blk_bm->bits, sizeof(size_t), 1, file);
//     fwrite(&blk_bm->size, sizeof(size_t), 1, file);
//     remaining -= (2 * sizeof(size_t));
//     fwrite(blk_bm->buf, 1, blk_bm->size, file);
//     remaining -= blk_bm->size;
//     free(blk_bm);

//     // Compute the block count
//     size_t block_area = block_count * BLOCK_SIZE;
//     if (remaining < block_area) {
//         log_error("no enough space left for blocks");
//         fclose(file);
//         return 1;
//     }
//     log_debug("mkfs: block area: %lu", block_count * BLOCK_SIZE);
//     remaining -= block_count * BLOCK_SIZE;

//     log_debug("unused: %u bytes", remaining);
//     log_debug("successfully created disk file");
//     fclose(file);
//     return 0;
// }

// void cl_find_dirent_with_parent(const char *path, Dirent *root,
//                                 Dirent **result, Dirent **parent) {
//     assert(root != NULL);
//     assert(path != NULL);

//     *parent = NULL;
//     *result = NULL;

//     log_debug("find %s -> start", path);

//     if (strcmp(path, PATH_SEP) == 0) {
//         log_debug("find %s -> root", path);
//         *result = root;
//     }

//     Dirent *cur = root;
//     char *cpy = malloc(strlen(path) + 1);
//     strcpy(cpy, path);

//     char *fragment = strtok(cpy, PATH_SEP);

//     while (fragment) {
//         *parent = cur;
//         cur = cl_get_dirent(cur, fragment);
//         if (cur == NULL) {
//             free(cpy);
//             log_debug("find %s -> end (failure)", path);
//             *result = NULL;
//             return;
//         }

//         fragment = strtok(NULL, PATH_SEP);
//         if (fragment == NULL) {
//             free(cpy);
//             log_debug("[resolve] %s -> inode %s", path, cur->name);
//             *result = cur;
//             return;
//         }
//     }

//     free(cpy);
// }

// Dirent *cl_find_dirent(const char *path, Dirent *root) {
//     Dirent *result;
//     Dirent *parent;
//     cl_find_dirent_with_parent(path, root, &result, &parent);
//     return result;
// }

// Dirent *root;

// void cl_fsinit(Dirent *rootent) { root = rootent; }

// int cl_open_dev(const char *filename) {
//     FILE *file = fopen(filename, "r+");
//     if (file == NULL) {
//         log_error("could not open %s", filename);
//         return 1;
//     }

//     log_debug("checking magic number");

//     for (size_t i = 0; i < 6; i++) {
//         if ((char)fgetc(file) != MAGIC[i]) {
//             log_error("invalid magic number at offset %u", i);
//             return 1;
//         }
//     }

//     u_int16_t block_size;
//     fread(&block_size, sizeof(uint16_t), 1, file);
//     log_debug("block size: %hu", block_size);

//     // inode bitmap
//     size_t inode_bm_bits;
//     size_t inode_bm_bytes;
//     fread(&inode_bm_bits, sizeof(size_t), 1, file);
//     fread(&inode_bm_bytes, sizeof(size_t), 1, file);
//     bitmap *inode_bm = bm_alloc(inode_bm_bits);
//     fread(inode_bm->buf, inode_bm_bytes, 1, file);
//     log_debug("inode bitmap size: %lu", inode_bm_bits);

//     return 0;
// }

// int cl_read(const char *path, char *buf, size_t size, off_t offset,
//             struct fuse_file_info *fi) {
//     log_debug("[read] %s (bufsize: %zo)", path, size);

//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent != NULL) {
//         if (ent->type == S_IFREG) {
//             cool_inode *inode = cl_get_inode(ent->node.file->inode);
//             size_t filesize = inode->st->st_size;
//             log_debug("[read] copying content of %lo bytes", filesize);
//             cl_read_storage(buf, filesize, inode->first_blocks);
//             return filesize;
//         }
//         log_error("[read] not a regular file: %s", path);
//         return -1;
//     }

//     log_error("[read] no such file: %s", path);

//     return -1;
// }

// int cl_open(const char *path, struct fuse_file_info *fi) {
//     // TODO what should we do here ? see open(2)
//     log_debug("[open] %s", path);
//     return 0;
// }

// int cl_getattr(const char *path, struct stat *st) {
//     memset(st, 0, sizeof(struct stat));

//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent == NULL) {
//         log_debug("[getattr] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     if (ent->type == S_IFREG) {
//         cool_inode *inode = cl_get_inode(ent->node.file->inode);
//         memcpy(st, inode->st, sizeof(struct stat));
//         log_debug("[getattr] %s", path);
//         return 0;
//     }
//     if (ent->type == S_IFDIR) {
//         memcpy(st, ent->node.dir->stat, sizeof(struct stat));
//         log_debug("[getattr] %s", path);
//         return 0;
//     }

//     log_debug("[getattr] %s -> ENOENT", path);
//     return -ENOENT;
// }

// struct cool_inode *get_inode_safe(Dirent *ent) {
//     return cl_get_inode(ent->node.file->inode);
// }

// struct stat *cl_stat(Dirent *ent) {
//     switch (ent->type) {
//     case S_IFREG:
//         cool_inode *ino = cl_get_inode(ent->node.file->inode);
//         if (ino != NULL) {
//             return ino->st;
//         }
//         break;
//     case S_IFDIR:
//         return ent->node.dir->stat;
//     }

//     return NULL;
// }

// int cl_access(const char *path, int mode) {
//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent == NULL) {
//         log_debug("[access] %s -> ENOENT", path);
//         if (mode == F_OK) {
//             errno = ENOENT;
//             return -1;
//         }
//     }

//     // TODO implement proper access() logic (see access(2))
//     log_debug("[access] %s (mode %o) -> OK", path, mode);
//     return 0;
//     // struct stat *st = cl_stat(ent);
//     // mode_t curmode = st->st_mode;

//     // switch (mode) {
//     //     case R_OK:
//     //         return curmode & R_OK != 0
//     // }

//     // log_debug("[access] %s (current: %o, compared: %o)", path, st->st_mode,
//     // mode); if ((st->st_mode & mode) == mode) {
//     //     return 0;
//     // }

//     // errno = EACCES;
//     // return -1;
// }

// int cl_unlink(const char *path) {
//     Dirent *ent;
//     Dirent *parent;
//     cl_find_dirent_with_parent(path, root, &ent, &parent);

//     if (ent == NULL) {
//         log_debug("[unlink] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     struct stat *st = cl_stat(ent);

//     if (st->st_nlink == 1) {
//         log_debug("[unlink] %s -> freeing inode", path);
//         cool_inode *inode = get_inode_safe(ent);
//         for (size_t i = 0; i < 10; i++) {
//             cl_free_block(inode->first_blocks[i]);
//         }

//         cl_free_inode(inode);
//     }

//     cl_remove_entry(parent->node.dir, ent);

//     return 0;
// }

// int cl_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
//     Dirent *ent;
//     Dirent *parent;
//     cl_find_dirent_with_parent(path, root, &ent, &parent);

//     if (parent == NULL) {
//         errno = ENOENT;
//         return -1;
//     }

//     if (parent->type != S_IFDIR) {
//         errno = ENOTDIR;
//         return -1;
//     }

//     cool_inode *inode = cl_new_inode();
//     cl_add_file(parent->node.dir, "NEWFILE", cl_new_file(inode->st->st_ino));

//     return 0;
// }

// int cl_rmdir(const char *path) {
//     Dirent *ent;
//     Dirent *parent;
//     cl_find_dirent_with_parent(path, root, &ent, &parent);

//     if (ent == NULL) {
//         log_debug("[rmdir] %s -> ENOENT", path);
//         errno = ENOENT;
//         return -1;
//     }

//     if (ent->type != S_IFDIR) {
//         log_debug("[rmdir] %s -> ENOTDIR", path);
//         errno = ENOTDIR;
//         return -1;
//     }

//     if (ent->node.dir->entry_cnt > 0) {
//         log_debug("[rmdir] %s -> ENOTEMPTY", path);
//         errno = ENOTEMPTY;
//         return -1;
//     }

//     cl_remove_entry(parent->node.dir, ent);

//     log_debug("[rmdir] %s -> OK", path);

//     return 0;
// }

// int cl_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
//                off_t offset, struct fuse_file_info *fi) {
//     (void)offset;
//     (void)fi;

//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent == NULL) {
//         log_debug("[readdir] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     filler(buf, ".", NULL, 0);
//     filler(buf, "..", NULL, 0);

//     for (size_t i = 0; i < ent->node.dir->entry_cnt; i++) {
//         Dirent *child = ent->node.dir->entries[i];
//         if (child != NULL) {
//             filler(buf, child->name, cl_get_stat(child), 0);
//         } else {
//             log_warn("[readdir] NULL child");
//         }
//     }

//     log_debug("[readdir] %s", path);
//     return 0;
// }

// int cl_chown(const char *path, uid_t uid, gid_t gid) {
//     Dirent *ent = cl_find_dirent(path, root);
//     if (ent == NULL) {
//         log_error("[chmod] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     struct stat *st = cl_stat(ent);
//     st->st_uid = uid;
//     st->st_gid = gid;

//     return 0;
// }

// int cl_chmod(const char *path, mode_t mode) {
//     log_debug("[chmod] %s %o", path, mode);
//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent == NULL) {
//         log_error("[chmod] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     struct stat *st = cl_stat(ent);
//     st->st_mode = mode;

//     log_debug("[chmod] %s -> %o", path, mode);

//     return 0;
// }

// int cl_write(const char *path, const char *buf, size_t size, off_t offset,
//              struct fuse_file_info *fi) {

//     Dirent *ent = cl_find_dirent(path, root);

//     if (ent == NULL) {
//         log_error("[write] %s -> ENOENT", path);
//         return -ENOENT;
//     }

//     if (ent->type != S_IFREG) {
//         log_error("[write] %s -> not a regular file", path);
//         return -ENOENT;
//     }

//     cool_inode *inode = cl_get_inode(ent->node.file->inode);
//     cl_write_storage(buf, size, inode);

//     log_debug("[write] %s -> OK (%lu bytes)", path, size);

//     return size;
// }

#endif /* HAVE_LIBFUSE */