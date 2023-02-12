#include <stdio.h>

#include "log/log.h"

#include "fs.h"

void print_help() {
    puts("usage: mkcoolfs device");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    log_info("Begin formatting.");

    int ret = cl_mkfs(argv[1]);
    if (ret != 0) {
        log_error("Formatting failed.");
        return 1;
    }

    log_info("Formatting successful.");

    return 0;
}
