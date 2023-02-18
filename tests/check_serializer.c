#include "../config.h"

#include <check.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "../src/bitmap.h"
#include "../src/block.h"
#include "../src/inode.h"
#include "../src/serializer.h"

void create_zero_file(const char *path, size_t size) {
    FILE *file = fopen(path, "w");
    char *zeros = calloc(size, 1);
    fwrite(zeros, size, 1, file);
    fclose(file);
    free(zeros);
}

START_TEST(check_ser_file_inode) {
    const char *filename = "check_ser_file_inode.tmp";
    create_zero_file(filename, 1024);

    FILE *stream = fopen(filename, "r+");

    FileInode fi = {
        .size = 131,
        .block_count = 3,
        .blocks = { 1, 2, 3, 0, 0, 0, 0, 0 }
    };

    Inode inode = {
        .number = 2332,
        .mode = S_IFREG | 0444,

        .atime = time(NULL),
        .ctime = time(NULL),
        .mtime = time(NULL),

        .gid = 23,
        .uid = 111,

        .data = { .file = fi },
    };

    ser_inode(&inode, stream);
    rewind(stream);
    Inode *out = deser_inode(stream);

    ck_assert_int_eq(inode.number, out->number);
    ck_assert_int_eq(inode.atime, out->atime);
    ck_assert_int_eq(inode.ctime, out->ctime);
    ck_assert_int_eq(inode.mtime, out->mtime);
    ck_assert_int_eq(inode.gid, out->gid);
    ck_assert_int_eq(inode.uid, out->uid);
    ck_assert_int_eq(inode.mode, out->mode);
    ck_assert_int_eq(inode.data.file.block_count, out->data.file.block_count);
    ck_assert_int_eq(inode.data.file.size, out->data.file.size);
    ck_assert_mem_eq(inode.data.file.blocks, out->data.file.blocks, 3 * sizeof(blno_t));

    free(out);
    fclose(stream);
}
END_TEST

START_TEST(check_ser_fsinfo) {
    const char *filename = "check_ser_fsinfo.tmp";
    create_zero_file(filename, 1024);

    FILE *stream = fopen(filename, "r+");

    FilesystemInfo fi = {
        .version = 1,
        .block_size = 512,
        .flags = FS_COMPRESS | FS_ENCRYPT,
        .inode_count = 1024,
        .block_count = 4096,
    };

    ser_info(&fi, stream);

    rewind(stream);

    FilesystemInfo *out = deser_info(stream);

    ck_assert_int_eq(fi.version, out->version);
    ck_assert_int_eq(fi.block_size, out->block_size);
    ck_assert_int_eq(fi.flags, out->flags);
    ck_assert_int_eq(fi.block_count, out->block_count);
    ck_assert_int_eq(fi.inode_count, out->inode_count);

    free(out);
    fclose(stream);
}
END_TEST

START_TEST(check_ser_bitmap) {
    const size_t block_size = 16;
    const char *filename = "check_ser_bitmap.tmp";
    create_zero_file(filename, block_size * 2);

    FILE *stream = fopen(filename, "r+");

    bitmap *in = bm_alloc(32);

    ser_bitmap(in, stream);

    rewind(stream);

    bitmap *out = deser_bitmap(stream);

    ck_assert_int_eq(in->bits, out->bits);
    ck_assert_int_eq(in->size, out->size);
    ck_assert_mem_eq(in->buf, out->buf, in->size);

    free(in);
    fclose(stream);
}
END_TEST

START_TEST(check_deser_block) {
    const size_t block_size = 16;
    const char *filename = "check_deser_block.tmp";
    create_zero_file(filename, block_size * 2);
    FILE *stream = fopen(filename, "r+");

    Block b1 = {
        .content = "hello",
        .size = 5,
    };

    Block b2 = {
        .content = "block2",
        .size = 6,
    };

    ser_block(&b1, block_size, stream);
    ser_block(&b2, block_size, stream);

    fclose(stream);
    stream = fopen(filename, "r");

    Block *ob1 = deser_block(block_size, stream);
    Block *ob2 = deser_block(block_size, stream);

    ck_assert_int_eq(b1.size, ob1->size);
    ck_assert_mem_eq(b1.content, ob1->content, b1.size);
    ck_assert_int_eq(b2.size, ob2->size);
    ck_assert_mem_eq(b2.content, ob2->content, b2.size);

    fclose(stream);
    remove(filename);
}
END_TEST

START_TEST(check_ser_block) {
    size_t BLOCK_SIZE = 32;
    const char *filename = "check_ser_block.tmp";
    create_zero_file(filename, BLOCK_SIZE * 3);
    FILE *file = fopen(filename, "r+");

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

    ser_block(&b1, BLOCK_SIZE, file);
    ser_block(&b2, BLOCK_SIZE, file);
    ser_block(&b3, BLOCK_SIZE, file);

    fclose(file);

    file = fopen(filename, "r");

    char *buf = malloc(BLOCK_SIZE * 2);
    ck_assert_int_eq(fread(buf, BLOCK_SIZE * 2, 1, file), 1);

    fclose(file);

    const char expected[] = {
        0x07, 0x00, 'b', 'l', 'o', 'c', 'k',  ' ',  '1', 0,   0,   0,   0,
        0,    0,    0,   0,   0,   0,   0,    0,    0,   0,   0,   0,   0,
        0,    0,    0,   0,   0,   0,   0x0f, 0x00, 't', 'h', 'i', 's', ' ',
        'i',  's',  ' ', 'b', 'l', 'o', 'c',  'k',  ' ', '2', 0,   0,   0,
        0,    0,    0,   0,   0,   0,   0,    0,    0,   0,   0,   0,
    };
    ck_assert_mem_eq(buf, expected, 64);

    remove(filename);
}
END_TEST

Suite *serializer_suite(void) {
    Suite *s = suite_create("serializer");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_ser_block);
        tcase_add_test(tc, check_deser_block);
        tcase_add_test(tc, check_ser_bitmap);
        tcase_add_test(tc, check_ser_fsinfo);
        tcase_add_test(tc, check_ser_file_inode);
    }
    suite_add_tcase(s, tc);

    return s;
}
