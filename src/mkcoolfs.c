#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log/log.h"

#include "fs.h"
#include "serializer.h"

void print_help() {
    puts("usage: mkcoolfs COMMAND DEVICE");
    puts("  commands: format, verify");
}

int write_fs(FILE *file) {
    FilesystemInfo info = {
        .block_count = 1024,
        .block_size = 1024,
        .flags = FS_NONE,
        .inode_count = 512,
        .version = 1,
    };

    bitmap *block_bitmap = bm_alloc(info.block_count);
    bitmap *inode_bitmap = bm_alloc(info.inode_count);

    Filesystem fs = {
        .info = &info,
        .block_bitmap = block_bitmap,
        .inode_bitmap = inode_bitmap,
        .inode_table = malloc(sizeof(Inode *) * info.inode_count),
    };

    serialize_fs(&fs, file);

    free(block_bitmap);
    free(inode_bitmap);
    free(fs.inode_table);

    return 0;
}

int verify_fs(const char *path) {
    FILE *file = fopen(path, "r");
    if (!file) {
        log_error("%s: %s", path, strerror(errno));
        return errno;
    }

    struct stat st;
    stat(path, &st);

    Filesystem *fs = deserialize_fs(file);

    size_t block_used = 0;
    bitmap *bbm = fs->block_bitmap;
    size_t block_count = fs->info->block_count;

    for (size_t i = 0; i < block_count; i++) {
        if (bm_is_set(bbm, i)) {
            block_used++;
        }
    }

    size_t inode_used = 0;
    bitmap *ibm = fs->inode_bitmap;
    size_t inode_count = fs->info->inode_count;

    for (size_t i = 0; i < inode_count; i++) {
        if (bm_is_set(ibm, i)) {
            inode_used++;
        }
    }

    size_t total_size = st.st_size;
    size_t used_size = block_used * fs->info->block_size;
    float MB = 1.0 / (1024.0F * 1024.0F);

    log_info("file:              %s", path);
    log_info("version:           %lu", fs->info->version);
    log_info("block size:        %lu B", fs->info->block_size);
    log_info("blocks:            %lu/%lu", block_used, fs->info->block_count);
    log_info("inodes:            %lu/%lu", inode_used, fs->info->inode_count);
    log_info("file size:         %lu B (%5.1f MB)", total_size, (float)total_size * MB);
    log_info("used:              %lu B (%5.f MB)", used_size, (float)used_size * MB);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        print_help();
        return 1;
    }

    if (strcmp(argv[1], "format") == 0) {
        FILE *file = fopen(argv[2], "r+");
        if (!file) {
            log_error("%s: %s", argv[2], strerror(errno));
            return errno;
        }
        if (write_fs(file) != 0) {
            log_error("Formatting failed.");
            return 1;
        } else {
            log_info("Formatting successful.");
            return 0;
        }
    } else if (strcmp(argv[1], "verify") == 0) {
        if (verify_fs(argv[2]) != 0) {
            return errno;
        }
        return 0;
    }
}
