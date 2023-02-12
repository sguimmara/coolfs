#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

#include "dirent.h"

int push_null_at_end(const void *a, const void *b) {
    Dirent **aa = (Dirent**)a;
    Dirent **bb = (Dirent**)b;
    if (*aa == NULL && *bb != NULL) {
        return 1;
    }

    if (*bb == NULL && *aa != NULL) {
        return -1;
    }

    return 0;
}

Dirent *new_dirent(const char *name, ino_t ino, unsigned int type, uid_t uid, gid_t gid) {
    Dirent* result = malloc(sizeof(Dirent));

    result->name = strdup(name);
    result->ino = ino;
    result->child_count = 0;
    result->children = NULL;
    result->mode = type;

    time_t now = time(NULL);

    result->atime = now;
    result->ctime = now;
    result->mtime = now;

    result->gid = gid;
    result->uid = uid;

    return result;
}

void dirent_stat(const Dirent *dirent, struct stat *st) {
    assert(dirent != NULL);
    assert(st != NULL);

    st->st_ino = dirent->ino;
    st->st_size = dirent->size;
    st->st_atime = dirent->atime;
    st->st_ctime = dirent->ctime;
    st->st_mtime = dirent->mtime;
    st->st_mode = dirent->mode;
    st->st_uid = dirent->uid;
    st->st_gid = dirent->gid;
}

int add_child(Dirent *parent, Dirent *child) {
    if (!S_ISDIR(parent->mode)) {
        return ENOTDIR;
    }

    child->parent = parent;
    parent->child_count++;
    
    if (parent->children == NULL) {
        parent->children = malloc(sizeof(Dirent*));
        parent->children[0] = child;
    } else {
        parent->children = realloc(parent->children, parent->child_count * sizeof(Dirent*));
        parent->children[parent->child_count - 1] = child;
    }

    return 0;
}

int indexof(void **array, size_t size, void *elem) {
    for (size_t i = 0; i < size; i++) {
        if (array[i] == elem) {
            return (int)i;
        }
    }
    
    return -1;
}

int remove(Dirent *parent, Dirent *child) {
    assert(parent->child_count > 0);

    child->parent = NULL;
    parent->children[indexof((void**)parent->children, parent->child_count, child)] = NULL;
    qsort(parent->children, parent->child_count, sizeof(Dirent*), push_null_at_end);
    parent->children = realloc(parent->children, sizeof(Dirent*) * parent->child_count - 1);
    parent->child_count--;

    return 0;
}

Dirent *find_descendant(Dirent *root, const PathBuf *path) {
    if (path->count == 0) {
        return root;
    }

    Dirent* current = root;

    for (size_t i = 0; i < path->count; i++) {
        const char *fragment = path->fragments[i];
        if (!S_ISDIR(current->mode)) {
            errno = ENOTDIR;
            return NULL;
        }
        if (current->child_count > 0) {
            for (size_t i = 0; i < current->child_count; i++) {
                if (strcmp(current->children[i]->name, fragment) == 0) {
                    current = current->children[i];
                    break;
                }
            }
        } else {
            errno = ENOENT;
            return NULL;
        }
    }
    
    return current;
}
