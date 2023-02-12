#include <check.h>
#include <stdlib.h>

#include "../src/bitmap.h"
#include "../src/path.h"

PathBuf *pb(const char *path) {
    PathBuf *res = malloc(sizeof(PathBuf));
    int ret = parse_path(path, res);
    return res;
}

START_TEST(basename_handles_root) {
    ck_assert_str_eq("/", basename(pb("/")));
    ck_assert_str_eq("foo", basename(pb("/foo")));
}
END_TEST

START_TEST(parent_handles_root) {
    ck_assert_str_eq("/", parent(pb("/")));
    ck_assert_str_eq("/", parent(pb("/foo")));
}
END_TEST

START_TEST(parent_returns_the_before_last_fragment) {
    ck_assert_str_eq("bar", parent(pb("/foo/bar/baz")));
    ck_assert_str_eq("foo", parent(pb("/foo/bar/")));
}
END_TEST

START_TEST(parse_path_handle_root_dir) {
    PathBuf path;
    int ret = parse_path("/", &path);
    ck_assert_int_eq(0, ret);
    ck_assert_int_eq(0, path.count);
}
END_TEST

START_TEST(parse_path_handles_arbitrary_paths) {
    PathBuf path;
    int ret = parse_path("/foo/bar/baz", &path);
    ck_assert_int_eq(0, ret);
    ck_assert_int_eq(3, path.count);
    ck_assert_str_eq("foo", path.fragments[0]);
    ck_assert_str_eq("bar", path.fragments[1]);
    ck_assert_str_eq("baz", path.fragments[2]);
}
END_TEST

Suite *path_suite(void) {
    Suite *s = suite_create("path");

    TCase *tc_parse_path = tcase_create("parse_path");
    {
        tcase_add_test(tc_parse_path, parse_path_handle_root_dir);
        tcase_add_test(tc_parse_path, parse_path_handles_arbitrary_paths);
    }
    suite_add_tcase(s, tc_parse_path);

    TCase *tc_parent = tcase_create("parent");
    { tcase_add_test(tc_parent, parent_handles_root); }
    suite_add_tcase(s, tc_parent);

    TCase *tc_basename = tcase_create("basename");
    { tcase_add_test(tc_basename, basename_handles_root); }
    suite_add_tcase(s, tc_basename);

    return s;
}

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr = srunner_create(path_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}