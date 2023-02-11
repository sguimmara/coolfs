#include "../config.h"

#include <check.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

#include "../src/dir.h"

START_TEST(new_dirent_assigns_correct_values) {
    Dirent* ent = new_dirent("foo", 32, S_IFREG, 1000, 1002);

    ck_assert_int_eq(32, ent->ino);
    ck_assert_str_eq("foo", ent->name);
    ck_assert_int_eq(0, ent->child_count);
    ck_assert_int_eq(1000, ent->uid);
    ck_assert_int_eq(1002, ent->gid);
    ck_assert_ptr_null(ent->children);
    ck_assert_ptr_null(ent->parent);
}
END_TEST

START_TEST(dirent_stat_returns_correct_value) {
    Dirent* ent = new_dirent("foo", 32, S_IFREG, 1000, 1002);

    struct stat st;
    dirent_stat(ent, &st);

    ck_assert_int_eq(32, st.st_ino);
    ck_assert_int_eq(1000, st.st_uid);
    ck_assert_int_eq(1002, st.st_gid);
    ck_assert_int_eq(ent->atime, st.st_atime);
    ck_assert_int_eq(ent->mtime, st.st_mtime);
    ck_assert_int_eq(ent->ctime, st.st_ctime);
}
END_TEST

START_TEST(dirent_size_returns_zero_for_directories) {
    const char *name = "foobarbaz";
    Dirent* ent = new_dirent(name, 32, S_IFDIR, 1000, 1002);

    struct stat st;
    dirent_stat(ent, &st);

    ck_assert_int_eq(ent->size, 0);
    ck_assert_int_eq(ent->size, st.st_size);
}
END_TEST

START_TEST(add_child_sets_child_parent) {
    Dirent* parent = new_dirent("foo", 32, S_IFDIR, 1000, 1000);
    Dirent* child1 = new_dirent("foo", 32, S_IFDIR, 1000, 1000);
    Dirent* child2 = new_dirent("foo", 32, S_IFDIR, 1000, 1000);

    ck_assert_int_eq(0, add_child(parent, child1));
    ck_assert_int_eq(0, add_child(parent, child2));

    ck_assert_ptr_eq(child1->parent, parent);
    ck_assert_int_eq(2, parent->child_count);
    ck_assert_ptr_eq(child1, parent->children[0]);
    ck_assert_ptr_eq(child2, parent->children[1]);
}
END_TEST

START_TEST(add_child_returns_ENOTDIR_if_parent_is_not_directory) {
    Dirent* parent = new_dirent("foo", 32, S_IFREG, 1000, 1000);
    Dirent* child1 = new_dirent("foo", 32, S_IFDIR, 1000, 1000);

    ck_assert_int_eq(ENOTDIR, add_child(parent, child1));
}
END_TEST

PathBuf *mk_path(const char *path) {
    PathBuf *res = malloc(sizeof(PathBuf));
    parse_path(path, res);
    return res;
}

START_TEST(find_descendant_returns_correct_value_for_root) {
    Dirent* root = new_dirent("/", 1, S_IFDIR, 0, 0);
    Dirent* a = new_dirent("a", 2, S_IFDIR, 1000, 1000);
    Dirent* b = new_dirent("b", 3, S_IFDIR, 1000, 1000);
    Dirent* c = new_dirent("c", 4, S_IFDIR, 1000, 1000);
    Dirent* d = new_dirent("d", 5, S_IFDIR, 1000, 1000);
    Dirent* file = new_dirent("file", 6, __S_IFREG, 1000, 1000);

    // /a/b/c/d
    add_child(root, a);
    add_child(a, b);
    add_child(a, file);
    add_child(b, c);
    add_child(c, d);

    ck_assert_ptr_eq(root, find_descendant(root, mk_path("/")));
    ck_assert_ptr_eq(a, find_descendant(root, mk_path("/a")));
    ck_assert_ptr_eq(b, find_descendant(root, mk_path("/a/b")));
    ck_assert_ptr_eq(c, find_descendant(root, mk_path("/a/b/c")));
    ck_assert_ptr_eq(d, find_descendant(root, mk_path("/a/b/c/d")));

    // No child found -> ENOENT
    errno = 0;
    ck_assert_ptr_eq(NULL, find_descendant(root, mk_path("/a/b/c/d/e")));
    ck_assert_int_eq(ENOENT, errno);

    // Fragment is not a directory -> ENOTDIR
    errno = 0;
    ck_assert_ptr_eq(NULL, find_descendant(root, mk_path("/a/file/whatever")));
    ck_assert_int_eq(ENOTDIR, errno);
}
END_TEST

START_TEST(remove_child_updates_children_list_of_parent) {
    Dirent* parent = new_dirent("parent", 1, S_IFDIR, 1000, 1000);
    Dirent* child1 = new_dirent("child1", 2, S_IFDIR, 1000, 1000);
    Dirent* child2 = new_dirent("child2", 3, S_IFDIR, 1000, 1000);
    Dirent* child3 = new_dirent("child3", 4, S_IFDIR, 1000, 1000);

    add_child(parent, child1);
    add_child(parent, child2);
    add_child(parent, child3);

    ck_assert_int_eq(3, parent->child_count);
    ck_assert_ptr_eq(child1, parent->children[0]);
    ck_assert_ptr_eq(child2, parent->children[1]);
    ck_assert_ptr_eq(child3, parent->children[2]);

    remove(parent, child2);

    ck_assert_int_eq(2, parent->child_count);
    ck_assert_ptr_eq(child1, parent->children[0]);
    ck_assert_ptr_eq(child3, parent->children[1]);

    remove(parent, child1);

    ck_assert_int_eq(1, parent->child_count);
    ck_assert_ptr_eq(child3, parent->children[0]);

    remove(parent, child3);
    ck_assert_int_eq(0, parent->child_count);
}
END_TEST

Suite *Dirent_suite(void) {
    Suite *s = suite_create("Dirent");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, new_dirent_assigns_correct_values);
        tcase_add_test(tc, dirent_stat_returns_correct_value);
        tcase_add_test(tc, add_child_sets_child_parent);
        tcase_add_test(tc, add_child_returns_ENOTDIR_if_parent_is_not_directory);
        tcase_add_test(tc, remove_child_updates_children_list_of_parent);
        tcase_add_test(tc, dirent_size_returns_zero_for_directories);
        tcase_add_test(tc, find_descendant_returns_correct_value_for_root);
    }
    suite_add_tcase(s, tc);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr = srunner_create(Dirent_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}