#ifndef _COOLFS_SERIALIZER_H_
#define _COOLFS_SERIALIZER_H_

#include <stdio.h>

#include "bitmap.h"
#include "inode.h"
#include "block.h"
#include "fsinfo.h"

void serialize_fs(Filesystem *fs, FILE *stream);
Filesystem *deserialize_fs(FILE *stream);

void ser_info(FilesystemInfo *fi, FILE *st);
FilesystemInfo *deser_info(FILE *st);

void ser_block(const Block *block, size_t block_size, FILE *stream);
Block *deser_block(size_t block_size, FILE *stream);

void ser_bitmap(const bitmap *bitmap, FILE *stream);
bitmap *deser_bitmap(FILE *stream);

void ser_inode(Inode *inode, FILE *stream);
void ser_inodes(Inode **inodes, size_t count, bitmap *bitmap, FILE *stream);
Inode *deser_inode(FILE *stream);
Inode **deser_inodes(size_t count, bitmap *bitmap, FILE *stream);

#endif /* _COOLFS_SERIALIZER_H_ */