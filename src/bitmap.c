#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>

#include "bitmap.h"

bitmap *bm_alloc(size_t bits) {
    size_t bytes = (bits / 8) + (bits % 8 != 0 ? 1 : 0);

    char *buf = calloc(1, bytes);

    bitmap *result = malloc(sizeof(bitmap));

    result->bits = bits;
    result->buf = buf;
    result->size = bytes;

    return result;
}

void bm_set(bitmap *bm, size_t n) {
    size_t byte = n / 8;
    size_t bit = n % 8;

    bm->buf[byte] |= (1 << bit);
}

void bm_unset(bitmap *bm, size_t n) {
    size_t byte = n / 8;
    size_t bit = n % 8;

    uint8_t current = bm->buf[byte];

    uint8_t mask = 1 << bit;
    bm->buf[byte] = current & ~mask;
}

int bm_is_set(bitmap *bm, size_t n) {
    size_t byte = n / 8;
    size_t bit = n % 8;

    int ret = (bm->buf[byte] & (1 << bit)) != 0;
    return ret;
}

int bm_get(bitmap *bm, size_t *result) {
    for (size_t i = 0; i < bm->bits; i++) {
        if (!bm_is_set(bm, i)) {
            bm_set(bm, i);
            *result = i;
            return 0;
        }
    }

    return -1;
}

void bm_free(bitmap *bm) {
    free(bm->buf);
}
