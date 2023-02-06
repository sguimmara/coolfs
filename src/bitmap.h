#ifndef _BITMAP_H_
#define _BITMAP_H_

#include <stddef.h>

typedef struct bitmap {
    size_t bits; // Actual number of bits
    size_t size; // Size in bytes
    char *buf;
} bitmap;

/**
 * @brief Allocates a bitmap of SIZE bits.
 * 
 * @param size The number of bits.
 * @return bitmap* The created bitmap.
 */
bitmap *bm_alloc(size_t size);

/**
 * @brief Frees the bitmap.
 * 
 * @param bm The bitmap to free.
 */
void bm_free(bitmap* bm);

/**
 * @brief Sets the nth bit.
 * 
 * @param n The index of the bit to set.
 */
void bm_set(bitmap *bm, size_t n);

/**
 * @brief Unsets the nth bit.
 * 
 * @param bm 
 * @param n 
 */
void bm_unset(bitmap *bm, size_t n);

/**
 * @brief Checks the nth bit. 
 * 
 * @param bm The bitmap.
 * @param n The index of the bit to check.
 * @return 1 if the bit is set, 0 otherwise.
 */
int bm_is_set(bitmap *bm, size_t n);

/**
 * @brief Searches for the first unset bit, set it, then sets its index into RESULT.
 * 
 * @param bm 
 * @param result The index of the unset bit, if found. Otherwise remains unchanged.
 * @return int 0 if an unset bit was found, 1 otherwise.
 */
int bm_get(bitmap *bm, size_t *result);

#endif