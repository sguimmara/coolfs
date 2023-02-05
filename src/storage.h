#ifndef _STORAGE_H_
#define _STORAGE_H_

#if !defined(BLOCK_SIZE)
#define BLOCK_SIZE 1024
#endif

#include <stdio.h>
#include <sys/stat.h>

#define MAX_INODES 1024

typedef size_t blno_t;

/** A data block */
typedef struct block {
    blno_t no;
    size_t size;
    char *content;
} block;

/** A serialized inode */
typedef struct disk_inode {
    struct stat st;
    char *name;
    size_t name_len;
    ino_t parent;
} disk_inode;

/**
 * @brief Initializes a new filesystem into the provided file.
 *
 * @param file The persistent storage file.
 * @return int The status.
 */
int mkfs(FILE *file);

/**
 * @brief Creates a block with the specified data.
 *
 * @param data The block data.
 * @param size The data size.
 * @return block* The created block.
 */
block *make_block(const char *data, const size_t size);

/**
 * @brief Returns the block with the specified number.
 *
 * @param no The block number.
 * @return block* The block if found, otherwise NULL.
 */
block *get_block(blno_t no);

/**
 * @brief Writes the block in the storage.
 *
 * @param blk The block to write.
 */
void block_write(block *blk);

/**
 * @brief Writes the buffer in the storage, and returns an array of block
 * numbers.
 *
 * @param buf  The buffer to write.
 * @param size The buffer size.
 * @param block_count The number of blocks written.
 * @return blno_t* The array of block numbers.
 */
blno_t *storage_write(const char *data, const size_t size, size_t *block_count);

/**
 * @brief Reads the specified blocks, then writes their content into the buffer.
 *
 * @param buf The buffer to write.
 * @param size The size to write.
 * @param blks The blocks to read.
 * @param blk_count The number of blocks to read.
 * @return int The status.
 */
int storage_read(char *buf, size_t size, blno_t *blks, size_t blk_count);

#endif /* _STORAGE_H_ */
