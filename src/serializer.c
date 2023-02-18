#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "serializer.h"

const char MAGIC[] = "coolfs";

size_t read_u64(FILE *stream) {
    size_t out;
    assert(fread(&out, sizeof(size_t), 1, stream) == 1);
    return out;
}

void write_u64(size_t u64, FILE *stream) {
    assert(fwrite(&u64, sizeof(size_t), 1, stream) == 1);
}

void write_u8(uint8_t u8, FILE *stream) {
    assert(fwrite(&u8, sizeof(uint8_t), 1, stream) == 1);
}

uint8_t read_u8(FILE *stream) {
    uint8_t out;
    assert(fread(&out, sizeof(uint8_t), 1, stream) == 1);
    return out;
}

uint16_t read_u16(FILE *stream) {
    uint16_t out;
    assert(fread(&out, sizeof(uint16_t), 1, stream) == 1);
    return out;
}

char *read_buf(size_t size, FILE *stream) {
    char *buf = malloc(size);
    assert(fread(buf, size, 1, stream) == 1);
    return buf;
}

Filesystem *deserialize_fs(FILE *stream) {
    char *magic = malloc(sizeof(MAGIC));
    assert(fread(magic, sizeof(MAGIC), 1, stream) == 1);
    if (memcmp(magic, MAGIC, sizeof(MAGIC)) != 0) {
        return NULL;
    }

    FilesystemInfo *fi = deser_info(stream);

    bitmap *inode_bitmap = deser_bitmap(stream);
    Inode **inodes = deser_inodes(fi->inode_count, inode_bitmap, stream);
    bitmap *block_bitmap = deser_bitmap(stream);

    Filesystem *result = malloc(sizeof(Filesystem));

    result->block_bitmap = block_bitmap;
    result->info = fi;
    result->inode_table = inodes;
    result->inode_bitmap = inode_bitmap;

    return result;
}

void serialize_fs(Filesystem *fs, FILE *stream) {
    rewind(stream);
    assert(fwrite(MAGIC, sizeof(MAGIC), 1, stream) == 1);

    ser_info(fs->info, stream);
    ser_bitmap(fs->inode_bitmap, stream);
    ser_inodes(fs->inode_table, fs->info->inode_count, fs->inode_bitmap,
               stream);
    ser_bitmap(fs->block_bitmap, stream);
}

Inode **deser_inodes(size_t count, bitmap *bitmap, FILE *stream) {
    Inode **result = malloc(count * sizeof(Inode *));
    for (size_t i = 0; i < count; i++) {
        if (bm_is_set(bitmap, i)) {
            result[i] = deser_inode(stream);
        } else {
            result[i] = NULL;
            fseek(stream, sizeof(Inode), SEEK_CUR);
        }
    }

    return result;
}

void ser_inodes(Inode **inodes, size_t count, bitmap *bitmap, FILE *stream) {
    for (size_t i = 0; i < count; i++) {
        if (bm_is_set(bitmap, i)) {
            ser_inode(inodes[i], stream);
        } else {
            fseek(stream, sizeof(Inode), SEEK_CUR);
        }
    }
}

void ser_info(FilesystemInfo *fi, FILE *stream) {
    write_u64(fi->version, stream);
    write_u64(fi->block_size, stream);
    write_u8(fi->flags, stream);
    write_u64(fi->block_count, stream);
    write_u64(fi->inode_count, stream);

    fseek(stream, sizeof(fi->reserved), SEEK_CUR);
}

FilesystemInfo *deser_info(FILE *stream) {
    FilesystemInfo *fi = malloc(sizeof(FilesystemInfo));

    fi->version = read_u64(stream);
    fi->block_size = read_u64(stream);
    fi->flags = read_u8(stream);
    fi->block_count = read_u64(stream);
    fi->inode_count = read_u64(stream);

    fseek(stream, sizeof(fi->reserved), SEEK_CUR);

    return fi;
}

void ser_block(const Block *block, size_t block_size, FILE *stream) {
    fwrite(&block->size, sizeof(uint16_t), 1, stream);
    fwrite(block->content, block->size, 1, stream);
    if (block->size < block_size) {
        fseek(stream, block_size - block->size - sizeof(uint16_t), SEEK_CUR);
    }
}

Block *deser_block(size_t block_size, FILE *stream) {
    uint16_t size = read_u16(stream);
    char *buf = read_buf(size, stream);
    if (size < block_size) {
        fseek(stream, block_size - size - sizeof(uint16_t), SEEK_CUR);
    }

    Block *result = malloc(sizeof(Block));
    result->size = size;
    result->content = buf;

    return result;
}

void ser_bitmap(const bitmap *bitmap, FILE *stream) {
    fwrite(&bitmap->size, sizeof(size_t), 1, stream);
    fwrite(&bitmap->bits, sizeof(size_t), 1, stream);
    fwrite(bitmap->buf, bitmap->size, 1, stream);
}

bitmap *deser_bitmap(FILE *stream) {
    size_t size = read_u64(stream);
    size_t bits = read_u64(stream);
    char *buf = read_buf(size, stream);

    bitmap *result = malloc(sizeof(bitmap));

    result->bits = bits;
    result->size = size;
    result->buf = buf;

    return result;
}

void ser_inode(Inode *inode, FILE *stream) {
    assert(fwrite(inode, sizeof(Inode), 1, stream) == 1);
}

Inode *deser_inode(FILE *stream) {
    Inode *result = malloc(sizeof(Inode));
    assert(fread(result, sizeof(Inode), 1, stream) == 1);

    return result;
}
