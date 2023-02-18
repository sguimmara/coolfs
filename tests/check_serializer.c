#include "../config.h"

#include <check.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <stdio.h>

#include "../src/serializer.h"
#include "../src/block.h"

void create_zero_file(const char *path, size_t size) {
    FILE *file = fopen(path, "w");
    char *zeros = calloc(size, 1);
    fwrite(zeros, size, 1, file);
    fclose(file);
    free(zeros);
}

START_TEST(check_ser_init) {
    FILE *file = fopen("serializer.tmp", "w");

    SerializerState* st = ser_init(file, 8);

    ck_assert_int_eq(st->block_size, 8);
    ck_assert_ptr_eq(st->stream, file);

    free(st);
    fclose(file);
}
END_TEST

START_TEST(check_serialize_block) {
    size_t BLOCK_SIZE = 32;
    create_zero_file("serializer.tmp", BLOCK_SIZE * 3);
    FILE *file = fopen("serializer.tmp", "r+");

    SerializerState* st = ser_init(file, BLOCK_SIZE);

    Block b1 = {
        .no = 3232,
        .size = 7,
        .flags = -1,
        .content = "block 1",
    };

    Block b2 = {
        .no = 3232,
        .size = 15,
        .flags = -1,
        .content = "this is block 2",
    };

    Block b3 = {
        .no = 3232,
        .size = 0,
        .flags = -1,
    };

    ser_block(&b1, st);
    ser_block(&b2, st);
    ser_block(&b3, st);

    free(st);
    fclose(file);

    file = fopen("serializer.tmp", "r");

    char *buf = malloc(BLOCK_SIZE * 2);
    fread(buf, BLOCK_SIZE * 2, 1, file);

    fclose(file);

    const char expected[] =
    {
        0x07, 0x00, 'b', 'l', 'o', 'c', 'k', ' ', '1', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0x0f, 0x00, 't', 'h', 'i', 's', ' ', 'i', 's', ' ', 'b', 'l', 'o', 'c', 'k', ' ', '2', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };
    ck_assert_mem_eq(buf, expected, 64);
}
END_TEST

Suite *serializer_suite(void) {
    Suite *s = suite_create("serializer");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_serialize_block);
        // tcase_add_test(tc, check_ser_init);
    }
    suite_add_tcase(s, tc);

    return s;
}
