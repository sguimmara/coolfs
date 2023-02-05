#include "test.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/log/log.h"

size_t level = 0;

char *indent() {
    char *result = malloc(level + 1);
    for (size_t i = 0; i < level; i++) {
        result[i] = '\t';
    }

    result[level] = '\0';
    return result;
}

void pass(const char *test) {
    level++;
    printf("%s [pass] %s\n", indent(), test);
    level--;
}

void push_section(const char *name) {
    level++;
    size_t size = strlen(name) - 8 + 1;
    char *res = malloc(size);
    memcpy(res, name, size);
    res[size - 1] = '\0';

    printf("%s* %s\n", indent(), res);
}

void init_test(const char *name) {
    log_set_level(5);
    char *cpy = malloc(strlen(name) + 1);
    strcpy(cpy, name);
    char *tok = strtok(cpy, ".");
    printf("[ %s ]\n", tok);
}

void pop() { level--; }
