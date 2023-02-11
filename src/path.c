#include <string.h>
#include <stdlib.h>

#include "path.h"

static char *PATH_SEP = "/";

int parse_path(const char *buf, PathBuf *result) {
    size_t len = strlen(buf);

    if (len == 1 && buf[0] == '/') {
        result->count = 0;
        return 0;
    }

    size_t count = 0;
    for (size_t i = 0; i < len; ++i) {
        if (buf[i] == '/') {
            ++count;
        }
    }

    result->count = count;
    result->fragments = malloc(sizeof(char*) * count);

    char *cpy = malloc(strlen(buf) + 1);
    strcpy(cpy, buf);
    char *fragment = strtok(cpy, PATH_SEP);

    size_t i = 0;
    while (fragment) {
        char *fragcpy = malloc(strlen(fragment) + 1);
        strcpy(fragcpy, fragment);
        result->fragments[i++] = fragcpy;
        fragment = strtok(NULL, PATH_SEP);
    }

    free(cpy);
    return 0;
}

char *parent(const PathBuf *path) {
    if (path->count <= 1) {
        // root directory
        return "/";
    }

    return path->fragments[path->count - 2];
}

char *basename(const PathBuf *path) {
    if (path->count == 0) {
        // root directory
        return "/";
    }

    return path->fragments[path->count - 1];
}
