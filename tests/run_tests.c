#include <stdlib.h>
#include <check.h>
#include "check_all.h"

int main(void) {
    int number_failed;
    Suite *s;
    SRunner *sr = srunner_create(block_allocator_suite());
    srunner_add_suite(sr, bitmap_suite());
    srunner_add_suite(sr, fs_suite());
    srunner_add_suite(sr, inode_suite());
    srunner_add_suite(sr, path_suite());

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}