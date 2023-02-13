#include "../config.h"

#include <check.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "../src/inode.h"

PathBuf *mk_path(const char *path) {
    PathBuf *res = malloc(sizeof(PathBuf));
    parse_path(path, res);
    return res;
}

START_TEST(check_get_inode_by_path) {
    inode_init();

    Inode *root = create_filesystem_root();
    Inode *a = create_directory(root);
    Inode *b = create_directory(a);
    Inode *c = create_directory(b);
    Inode *d = create_directory(b);
    Inode *file = create_file(a);

    add_entry(root, "a", a->number);
    add_entry(a, "b", b->number);
    add_entry(b, "c", c->number);
    add_entry(b, "d", d->number);
    add_entry(a, "file", file->number);

    ck_assert_ptr_eq(root, get_inode_by_pathbuf(mk_path("/")));
    ck_assert_ptr_eq(a, get_inode_by_pathbuf(mk_path("/a")));
    ck_assert_ptr_eq(b, get_inode_by_pathbuf(mk_path("/a/b")));
    ck_assert_ptr_eq(c, get_inode_by_pathbuf(mk_path("/a/b/c")));
    ck_assert_ptr_eq(d, get_inode_by_pathbuf(mk_path("/a/b/d")));

    // No child found -> ENOENT
    errno = 0;
    ck_assert_ptr_null(get_inode_by_pathbuf(mk_path("/a/b/c/d/e")));
    ck_assert_int_eq(ENOENT, errno);

    // Fragment is not a directory -> ENOTDIR
    errno = 0;
    ck_assert_ptr_null(get_inode_by_pathbuf(mk_path("/a/file/whatever")));
    ck_assert_int_eq(ENOTDIR, errno);
}
END_TEST

START_TEST(check_create_filesystem_root) {
    inode_init();

    Inode *root = create_filesystem_root();

    ck_assert_int_eq(0, root->number);
    ck_assert_int_eq(1, S_ISDIR(root->mode));
    ck_assert_int_eq(0, root->uid);
    ck_assert_int_eq(0, root->gid);

    ck_assert_int_eq(0, root->data.dir.entry_count);
}
END_TEST

START_TEST(check_create_directory) {
    inode_init();

    Inode *root = create_filesystem_root();

    root->gid = 1;
    root->uid = 2;

    Inode* dir = create_directory(root);

    ck_assert_int_eq(1, dir->number);
    ck_assert_int_eq(1, S_ISDIR(root->mode));

    // Check that the dir inherits its parent UID/GID
    ck_assert_int_eq(root->uid, dir->uid);
    ck_assert_int_eq(root->gid, dir->gid);

    ck_assert_int_eq(0, dir->data.dir.entry_count);
}
END_TEST

START_TEST(check_create_file) {
    inode_init();

    Inode *root = create_filesystem_root();

    root->gid = 1;
    root->uid = 2;

    Inode* file = create_file(root);

    ck_assert_int_eq(1, file->number);
    ck_assert_int_eq(1, S_ISREG(file->mode));

    // Check that the dir inherits its parent UID/GID
    ck_assert_int_eq(root->uid, file->uid);
    ck_assert_int_eq(root->gid, file->gid);

    ck_assert_int_eq(0, file->data.file.size);
    ck_assert_int_eq(0, file->data.file.block_count);
}
END_TEST

START_TEST(check_add_remove_entry) {
    inode_init();

    Inode *root = create_filesystem_root();

    add_entry(root, "a", 1);
    add_entry(root, "b", 2);
    add_entry(root, "c", 3);

    ck_assert_int_eq(3, root->data.dir.entry_count);
    ck_assert_str_eq("a", root->data.dir.entries[0].name);
    ck_assert_int_eq(1, root->data.dir.entries[0].inode);

    ck_assert_str_eq("b", root->data.dir.entries[1].name);
    ck_assert_int_eq(2, root->data.dir.entries[1].inode);

    ck_assert_str_eq("c", root->data.dir.entries[2].name);
    ck_assert_int_eq(3, root->data.dir.entries[2].inode);

    remove_entry(root, 1);
    ck_assert_int_eq(2, root->data.dir.entry_count);

    ck_assert_str_eq("c", root->data.dir.entries[0].name);
    ck_assert_int_eq(3, root->data.dir.entries[0].inode);

    ck_assert_str_eq("b", root->data.dir.entries[1].name);
    ck_assert_int_eq(2, root->data.dir.entries[1].inode);
}
END_TEST

START_TEST(check_stat_inode) {
    inode_init();

    Inode *root = create_filesystem_root();

    struct stat st;
    stat_inode(root, &st);

    ck_assert_int_eq(root->number, st.st_ino);
    ck_assert_int_eq(root->mode, st.st_mode);
    ck_assert_int_eq(root->atime, st.st_atime);
    ck_assert_int_eq(root->ctime, st.st_ctime);
    ck_assert_int_eq(root->mtime, st.st_mtime);
}
END_TEST

START_TEST(check_get_inode) {
    inode_init();

    Inode *root = create_filesystem_root();

    add_entry(root, "foo", 32323);
    add_entry(root, "bar", 1123);
    add_entry(root, "baz", 31029);

    ck_assert_int_eq(3, root->data.dir.entry_count);

    DirEnt ent0 = root->data.dir.entries[0];
    ck_assert_int_eq(32323, ent0.inode);
    ck_assert_str_eq("foo", ent0.name);

    DirEnt ent1 = root->data.dir.entries[1];
    ck_assert_int_eq(1123, ent1.inode);
    ck_assert_str_eq("bar", ent1.name);

    DirEnt ent2 = root->data.dir.entries[2];
    ck_assert_int_eq(31029, ent2.inode);
    ck_assert_str_eq("baz", ent2.name);
}
END_TEST

Suite *suite(void) {
    Suite *s = suite_create("inode");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, check_get_inode_by_path);
        tcase_add_test(tc, check_create_filesystem_root);
        tcase_add_test(tc, check_create_directory);
        tcase_add_test(tc, check_create_file);
        tcase_add_test(tc, check_stat_inode);
        tcase_add_test(tc, check_add_remove_entry);
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