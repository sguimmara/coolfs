#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "test.h"

#include "../src/bitmap.h"

void bm_alloc_creates_correct_size() {

    size_t size = 19199191;
    bitmap *bm = bm_alloc(size);

    assert(bm->bits == size);
    assert(bm->size >= size / 8);

    for (size_t i = 0; i < bm->size; i++) {
        char c = (char)bm->buf[i];
        assert(c == 0);
    }

    pass(__FUNCTION__);
}

void bm_set_sets_the_nth_bit() {
    for (size_t byte = 0; byte < 64; byte++) {
        for (size_t bit = 0; bit < 8; bit++) {
            bitmap *bm = bm_alloc(64 * 8);

            size_t n = byte * 8 + bit;

            assert((uint8_t)bm->buf[byte] == 0);

            assert(bm_is_set(bm, n) == 0);

            bm_set(bm, n);

            assert((uint8_t)bm->buf[byte] == (1 << bit));

            assert(bm_is_set(bm, n) == 1);

            bm_free(bm);
        }
    }

    pass(__FUNCTION__);
}

void bm_get_returns_correct_value() {
    bitmap *bm = bm_alloc(32);

    size_t result;

    for (size_t i = 0; i < 32; i++) {
        assert(bm_is_set(bm, i) == 0);
    }

    for (size_t i = 0; i < 32; i++) {
        int ret = bm_get(bm, &result);
        assert(ret == 0);
        assert(result == i);
        assert(bm_is_set(bm, result) == 1);
    }

    assert(bm_get(bm, &result) != 1);

    bm_unset(bm, 9);

    bm_get(bm, &result);

    assert(result == 9);

    pass(__FUNCTION__);
}

int main(int argc, char **argv) {
    init_test(__FILE__);

    bm_set_sets_the_nth_bit();
    bm_get_returns_correct_value();

    return 0;
}
