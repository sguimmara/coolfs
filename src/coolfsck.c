#include <stdio.h>

#include "log/log.h"

#include "fs.h"

void print_help() {
    puts("usage: coolfsck device");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    int ret = cl_open_dev(argv[1]);
    if (ret != 0) {
        log_error("Check failed.");
        return 1;
    }

    log_info("Check successful.");

    return 0;
}