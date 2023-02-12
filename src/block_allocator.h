// #ifndef _STORAGE_H_
// #define _STORAGE_H_

// #include <stdio.h>
// #include <sys/stat.h>

// #include "constants.h"
// #include "inode.h"

// /** A data block */
// typedef struct block {
//     size_t no;
//     size_t size;
//     char *content;
// } block;

// void cl_init_storage(FILE *file);
// void cl_storage_dispose();

// /**
//  * @brief Creates a block with the specified data.
//  *
//  * @param data The block data.
//  * @param size The data size.
//  * @return block* The created block.
//  */
// block *cl_new_block(const char *data, const size_t size);

// /**
//  * @brief Returns the block with the specified number.
//  *
//  * @param no The block number.
//  * @return block* The block if found, otherwise NULL.
//  */
// block *cl_get_block(size_t no);

// /**
//  * @brief Frees the block.
//  *
//  * @param no The block number.
//  */
// void cl_free_block(size_t no);

// /**
//  * @brief Writes the block in the storage.
//  *
//  * @param blk The block to write.
//  */
// void cl_write_block(block *blk);

// /**
//  * @brief Writes the buffer in the storage, and returns an array of block
//  * numbers.
//  *
//  * @param buf  The buffer to write.
//  * @param size The buffer size.
//  * @param inode The inode to update with block information.
//  * @return int The status
//  */
// int cl_write_storage(const char *data, const size_t size, cool_inode *inode);

// /**
//  * @brief Reads the specified blocks, then writes their content into the buffer.
//  *
//  * @param buf The buffer to write.
//  * @param size The size to write.
//  * @param blks The blocks to read.
//  * @param blk_count The number of blocks to read.
//  * @return int The status.
//  */
// int cl_read_storage(char *buf, size_t size, size_t *blks);

// #endif /* _STORAGE_H_ */
