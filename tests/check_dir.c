#include <check.h>
#include <stdlib.h>

#include "../src/dir.h"

START_TEST(Dirent_root) {
    
}
END_TEST

Suite *Dirent_suite(void) {
    Suite *s = suite_create("Dirent");

    TCase *tc = tcase_create("core");
    {
        tcase_add_test(tc, Dirent_root);
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