#include "../config.h"

#include <check.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "../src/fs.h"
#include "../src/inode.h"

START_TEST(check_getattr) {
    fs_init();

    Inode *dir = create_directory(get_root());
    add_entry(get_root(), "myDir", dir->number);

    dir->uid = 23;
    dir->gid = 1441;

    struct stat st;

    _getattr("/myDir", &st);

    ck_assert_int_eq(dir->number, st.st_ino);
    ck_assert_int_eq(dir->mode, st.st_mode);
    ck_assert_int_eq(dir->atime, st.st_atime);
    ck_assert_int_eq(dir->mtime, st.st_mtime);
    ck_assert_int_eq(dir->ctime, st.st_ctime);
    ck_assert_int_eq(dir->gid, st.st_gid);
    ck_assert_int_eq(dir->uid, st.st_uid);
}
END_TEST

START_TEST(check_chown) {
    fs_init();

    Inode *dir = create_directory(get_root());
    add_entry(get_root(), "myDir", dir->number);

    _chown("/myDir", 111, 222);

    ck_assert_int_eq(111, dir->uid);
    ck_assert_int_eq(222, dir->gid);

    struct stat st;
    _getattr("/myDir", &st);

    ck_assert_int_eq(dir->gid, st.st_gid);
    ck_assert_int_eq(dir->uid, st.st_uid);
}
END_TEST

START_TEST(check_chmod) {
    fs_init();

    Inode *dir = create_directory(get_root());
    add_entry(get_root(), "myDir", dir->number);

    int new_mode = S_IRUSR | S_IXGRP | S_IWOTH;
    _chmod("/myDir", new_mode);

    ck_assert_int_eq(1, S_ISDIR(dir->mode));
    ck_assert_int_eq(1, (dir->mode & S_IRWXU) == S_IRUSR);
    ck_assert_int_eq(1, (dir->mode & S_IRWXG) == S_IXGRP);
    ck_assert_int_eq(1, (dir->mode & S_IRWXO) == S_IWOTH);
}
END_TEST

Suite *suite(void) {
    Suite *s = suite_create("Dirent");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_getattr);
        tcase_add_test(tc, check_chown);
        tcase_add_test(tc, check_chmod);
    }
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr = srunner_create(suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}