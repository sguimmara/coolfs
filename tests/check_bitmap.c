#include <check.h>
#include <stdlib.h>

#include "../src/bitmap.h"

START_TEST(bm_alloc_creates_correct_size) {
    size_t size = 19199191;
    bitmap *bm = bm_alloc(size);

    ck_assert_int_eq(bm->bits, size);
    ck_assert_int_ge(bm->size, size / 8);

    for (size_t i = 0; i < bm->size; i++) {
        char c = (char)bm->buf[i];
        ck_assert_int_eq(c, 0);
    }
}
END_TEST

START_TEST(bm_set_sets_the_nth_bit) {
    for (size_t byte = 0; byte < 64; byte++) {
        for (size_t bit = 0; bit < 8; bit++) {
            bitmap *bm = bm_alloc(64 * 8);

            size_t n = byte * 8 + bit;

            ck_assert_int_eq((uint8_t)bm->buf[byte], 0);

            ck_assert_int_eq(bm_is_set(bm, n), 0);

            bm_set(bm, n);

            ck_assert_int_eq((uint8_t)bm->buf[byte], (1 << bit));

            ck_assert_int_eq(bm_is_set(bm, n), 1);

            bm_free(bm);
        }
    }
}
END_TEST

START_TEST(bm_get_returns_correct_value) {
    bitmap *bm = bm_alloc(32);

    size_t result;

    for (size_t i = 0; i < 32; i++) {
        ck_assert_int_eq(bm_is_set(bm, i), 0);
    }

    for (size_t i = 0; i < 32; i++) {
        int ret = bm_get(bm, &result);
        ck_assert_int_eq(ret, 0);
        ck_assert_int_eq(result, i);
        ck_assert_int_eq(bm_is_set(bm, result), 1);
    }

    ck_assert_int_ne(bm_get(bm, &result), 1);

    bm_unset(bm, 9);

    bm_get(bm, &result);

    ck_assert_int_eq(result, 9);
}
END_TEST

Suite *bitmap_suite(Suite *suite) {
    Suite *s = suite_create("bitmap");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, bm_get_returns_correct_value);
        tcase_add_test(tc, bm_set_sets_the_nth_bit);
        tcase_add_test(tc, bm_alloc_creates_correct_size);
    }
    suite_add_tcase(s, tc);

    return s;
}
