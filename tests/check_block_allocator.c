#include <check.h>
#include <errno.h>
#include <stdlib.h>

#include "../src/block.h"
#include "../src/block_allocator.h"

START_TEST(check_read_from_blocks) {
    const char *data = "AAAABBBBCCCCDD";
    size_t size = strlen(data);
    size_t block_size = 4;

    block_allocator_init(block_size, 10);

    size_t count;
    blno_t *blck_nums = allocate_blocks(size, &count);

    write_into_blocks(data, size, blck_nums, count);

    char *dst = malloc(128);

    read_from_blocks(dst, 128, blck_nums, count);

    ck_assert_int_eq(memcmp(data, dst, size), 0);

    free(blck_nums);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_write_into_blocks) {
    const char *data = "AAAABBBBCCCCDD";
    size_t size = strlen(data);
    size_t block_size = 4;

    block_allocator_init(block_size, 10);

    size_t count;
    blno_t *blck_nums = allocate_blocks(size, &count);

    ck_assert_int_eq(count, 4);

    write_into_blocks(data, size, blck_nums, count);

    Block *b0 = get_block(blck_nums[0]);
    Block *b1 = get_block(blck_nums[1]);
    Block *b2 = get_block(blck_nums[2]);
    Block *b3 = get_block(blck_nums[3]);

    ck_assert_int_eq(b0->size, block_size);
    ck_assert_int_eq(b1->size, block_size);
    ck_assert_int_eq(b2->size, block_size);
    ck_assert_int_eq(b3->size, 2);

    ck_assert_int_eq(memcmp("AAAA", b0->content, block_size), 0);
    ck_assert_int_eq(memcmp("BBBB", b1->content, block_size), 0);
    ck_assert_int_eq(memcmp("CCCC", b2->content, block_size), 0);
    ck_assert_int_eq(memcmp("DD", b3->content, 2), 0);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_allocate_blocks_ENOSPC) {
    block_allocator_init(128, 10);

    size_t count;
    ck_assert_ptr_null(allocate_blocks(9999999, &count));
    ck_assert_int_eq(errno, ENOSPC);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_allocate_blocks) {
    block_allocator_init(128, 10);

    size_t count = -1;
    blno_t *result;

    result = allocate_blocks(128, &count);
    ck_assert_int_eq(count, 1);
    ck_assert_int_eq(result[0], 0);
    release_block(result[0]);
    free(result);

    result = allocate_blocks(129, &count);
    ck_assert_int_eq(count, 2);
    ck_assert_int_eq(result[0], 0);
    ck_assert_int_eq(result[1], 1);
    release_block(result[0]);
    release_block(result[1]);
    free(result);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_new_block) {
    block_allocator_init(128, 1);

    Block *block = new_block();
    ck_assert_ptr_nonnull(block);
    ck_assert_int_eq(block->size, 0);
    ck_assert_int_eq(block->no, 0);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_get_stats) {
    block_allocator_init(128, 10);

    Block *b1 = new_block();

    DiskUsage du;

    du = get_stats();
    ck_assert_int_eq(du.block_size, 128);
    ck_assert_int_eq(du.allocated_blocks, 1);
    ck_assert_int_eq(du.total_capacity, 10 * 128);
    ck_assert_int_eq(du.used_space, 128);

    release_block(b1->no);

    du = get_stats();
    ck_assert_int_eq(du.block_size, 128);
    ck_assert_int_eq(du.allocated_blocks, 0);
    ck_assert_int_eq(du.total_capacity, 10 * 128);
    ck_assert_int_eq(du.used_space, 0);

    Block *b2 = new_block();
    Block *b3 = new_block();
    Block *b4 = new_block();
    Block *b5 = new_block();

    du = get_stats();
    ck_assert_int_eq(du.block_size, 128);
    ck_assert_int_eq(du.allocated_blocks, 4);
    ck_assert_int_eq(du.total_capacity, 10 * 128);
    ck_assert_int_eq(du.used_space, 4 * 128);

    block_allocator_destroy();
}
END_TEST

START_TEST(check_release_block) {
    block_allocator_init(128, 1);

    Block *block1 = new_block();
    block1->size = 33;
    ck_assert_ptr_eq(get_block(block1->no), block1);

    release_block(block1->no);

    Block *block2 = new_block();

    ck_assert_int_eq(block2->size, 0);
    ck_assert_int_eq(block2->no, 0);

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

START_TEST(check_get_block) {
    block_allocator_init(128, 1024);

    Block *b0 = new_block();
    Block *b1 = new_block();
    Block *b2 = new_block();
    Block *b3 = new_block();
    Block *b4 = new_block();

    ck_assert_int_eq(b0->no, 0);
    ck_assert_int_eq(b1->no, 1);
    ck_assert_int_eq(b2->no, 2);
    ck_assert_int_eq(b3->no, 3);
    ck_assert_int_eq(b4->no, 4);

    ck_assert_ptr_eq(get_block(b0->no), b0);
    ck_assert_ptr_eq(get_block(b1->no), b1);
    ck_assert_ptr_eq(get_block(b2->no), b2);
    ck_assert_ptr_eq(get_block(b3->no), b3);
    ck_assert_ptr_eq(get_block(b4->no), b4);

    block_allocator_destroy();
}
END_TEST

Suite *block_allocator_suite(void) {
    Suite *s = suite_create("block_allocator");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_new_block);
        tcase_add_test(tc, check_new_block_returns_ENOSPC);
        tcase_add_test(tc, check_get_block);
        tcase_add_test(tc, check_release_block);
        tcase_add_test(tc, check_get_stats);
        tcase_add_test(tc, check_allocate_blocks);
        tcase_add_test(tc, check_allocate_blocks_ENOSPC);
        tcase_add_test(tc, check_write_into_blocks);
        tcase_add_test(tc, check_read_from_blocks);
    }
    suite_add_tcase(s, tc);

    return s;
}
