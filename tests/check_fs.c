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

START_TEST(check_unlink) {
    fs_init();

    // /
    //  file
    //  dir/
    Inode *dir = create_directory(get_root());
    add_entry(get_root(), "dir", dir->number);

    Inode *file = create_file(get_root());
    add_entry(get_root(), "file", file->number);

    ino_t dirno = dir->number;
    ino_t fileno = file->number;

    ck_assert_int_eq(_unlink("/"), -EISDIR);

    ck_assert_int_eq(_unlink("/dir"), -EISDIR);

    ck_assert_int_eq(get_root()->data.dir.entry_count, 2);
    ck_assert_int_eq(_unlink("/file"), 0);
    ck_assert_int_eq(get_root()->data.dir.entry_count, 1);

    ck_assert_ptr_null(get_inode(fileno));
}
END_TEST

START_TEST(check_create) {
    fs_init();

    ck_assert_int_eq(_create("/file0", 0500), 0);
    ck_assert_int_eq(_mkdir("/dir", 0444), 0);
    ck_assert_int_eq(_mkdir("/dir/subdir", 0444), 0);
    ck_assert_int_eq(_create("/dir/file1", 0555), 0);
    ck_assert_int_eq(_create("/dir/subdir/file2", 0777), 0);

    Inode *file0 = get_inode_by_pathbuf(parse_path_safe("/file0"));
    ck_assert_ptr_nonnull(file0);
    ck_assert_int_eq(file0->mode, S_IFREG | 0500);

    Inode *file1 = get_inode_by_pathbuf(parse_path_safe("/dir/file1"));
    ck_assert_ptr_nonnull(file1);
    ck_assert_int_eq(file1->mode, S_IFREG | 0555);

    Inode *file2 = get_inode_by_pathbuf(parse_path_safe("/dir/subdir/file2"));
    ck_assert_ptr_nonnull(file2);
    ck_assert_int_eq(file2->mode, S_IFREG | 0777);
}
END_TEST

START_TEST(check_rename) {
    fs_init();

    _create("/src", S_IFDIR | 0444);

    Inode *inode = get_inode_by_pathbuf(parse_path_safe("/src"));

    ck_assert_int_eq(_rename("/foo", "/dst"), -ENOENT);
    ck_assert_int_eq(_rename("/src", "/dst"), 0);

    ck_assert_ptr_null(get_inode_by_pathbuf(parse_path_safe("/src")));
    ck_assert_ptr_eq(get_inode_by_pathbuf(parse_path_safe("/dst")), inode);
}
END_TEST

START_TEST(check_mkdir) {
    fs_init();

    ck_assert_int_eq(_mkdir("/dir", 0555), 0);
    ck_assert_int_eq(_mkdir("/dir/a", 0444), 0);
    ck_assert_int_eq(_mkdir("/dir/a/b", 0777), 0);

    ck_assert_int_eq(_mkdir("/dir/X/b", 0777), -ENOENT);

    Inode *dir = get_inode_by_pathbuf(parse_path_safe("/dir"));
    Inode *a = get_inode_by_pathbuf(parse_path_safe("/dir/a"));
    Inode *b = get_inode_by_pathbuf(parse_path_safe("/dir/a/b"));

    ck_assert_int_eq(dir->mode, S_IFDIR | 0555);
    ck_assert_int_eq(a->mode, S_IFDIR | 0444);
    ck_assert_int_eq(b->mode, S_IFDIR | 0777);
}
END_TEST

START_TEST(check_utimens) {
    fs_init();

    ck_assert_int_eq(_mkdir("/dir", 0555), 0);
    ck_assert_int_eq(_create("/dir/a", 0444), 0);

    Inode *dir = get_inode_by_pathbuf(parse_path_safe("/dir"));
    Inode *a = get_inode_by_pathbuf(parse_path_safe("/dir/a"));

    struct timespec dirtime[2] = { { .tv_sec = 3232 }, { .tv_sec = 11111 }};
    struct timespec atime[2] = { { .tv_sec = 1212 }, { .tv_sec = 9999 }};
    ck_assert_int_eq(_utimens("/dir", dirtime), 0);
    ck_assert_int_eq(_utimens("/dir/a", atime), 0);
    ck_assert_int_eq(_utimens("/noexists", atime), -ENOENT);

    ck_assert_int_eq(dir->atime, 3232);
    ck_assert_int_eq(dir->mtime, 11111);
    ck_assert_int_eq(a->atime, 1212);
    ck_assert_int_eq(a->mtime, 9999);
}
END_TEST

START_TEST(check_rmdir) {
    fs_init();

    // /
    //  file
    //  dir/
    //    subdir/
    Inode *dir = create_directory(get_root());
    add_entry(get_root(), "dir", dir->number);

    Inode *file = create_file(get_root());
    add_entry(get_root(), "file", file->number);

    Inode *subdir = create_directory(dir);
    add_entry(dir, "subdir", subdir->number);

    ino_t dirno = dir->number;
    ino_t subdirno = subdir->number;
    ino_t fileno = file->number;

    ck_assert_int_eq(_rmdir("/"), -EACCES);

    ck_assert_int_eq(_rmdir("/dir"), -ENOTEMPTY);

    ck_assert_int_eq(_rmdir("/file"), -ENOTDIR);

    ck_assert_int_eq(_rmdir("/dir/subdir"), 0);
    ck_assert_int_eq(dir->data.dir.entry_count, 0);

    ck_assert_int_eq(get_root()->data.dir.entry_count, 2);
    ck_assert_int_eq(_rmdir("/dir"), 0);
    ck_assert_int_eq(get_root()->data.dir.entry_count, 1);

    ck_assert_ptr_eq(get_inode(get_root()->number), get_root());
    ck_assert_ptr_null(get_inode(dirno));
    ck_assert_ptr_null(get_inode(subdirno));
    ck_assert_ptr_eq(get_inode(fileno), file);
}
END_TEST

Suite *fs_suite(void) {
    Suite *s = suite_create("fs");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_getattr);
        tcase_add_test(tc, check_chown);
        tcase_add_test(tc, check_chmod);
        tcase_add_test(tc, check_rmdir);
        tcase_add_test(tc, check_unlink);
        tcase_add_test(tc, check_create);
        tcase_add_test(tc, check_rename);
        tcase_add_test(tc, check_mkdir);
        tcase_add_test(tc, check_utimens);
    }
    suite_add_tcase(s, tc);

    return s;
}
