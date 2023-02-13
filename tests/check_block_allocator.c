#include <check.h>
#include <stdlib.h>
#include <errno.h>

#include "../src/block.h"
#include "../src/block_allocator.h"

START_TEST(check_new_block) {
    block_allocator_init(128, 1);

    Block *block = new_block();
    ck_assert_ptr_nonnull(block);
    ck_assert_int_eq(block->size, 0);
    ck_assert_int_eq(block->no, 0);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_new_block_returns_ENOSPC) {
    block_allocator_init(128, 1);

    errno = 0;
    Block *block1 = new_block();
    ck_assert_int_eq(errno, 0);
    Block *block2 = new_block();
    ck_assert_int_eq(errno, ENOSPC);

    ck_assert_ptr_nonnull(block1);
    ck_assert_ptr_null(block2);

    block_allocator_destroy();
}
END_TEST

Suite *block_allocator_suite(void) {
    Suite *s = suite_create("block_allocator");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_new_block);
        tcase_add_test(tc, check_new_block_returns_ENOSPC);
    }
    suite_add_tcase(s, tc);

    return s;
}
