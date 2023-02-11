#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <assert.h>

// #include "log/log.h"
// #include "inode_alloc.h"
#include "dir.h"

// struct stat *cl_get_stat(Dirent *dirent) {
//     switch (dirent->type) {
//         case S_IFREG:
//             cool_inode *inode = cl_get_inode(dirent->node.file->inode);
//             return inode->st;
//         case S_IFDIR:
//             return dirent->node.dir->stat;
//     }

//     log_error("fatal: trying to get stat on invalid object");
//     exit(1);
// }

// Dirent *cl_new_root() {
//     Dirent *res = malloc(sizeof(Dirent));
//     res->name = "/";
//     res->type = S_IFDIR;
//     res->node.dir = cl_new_dir();

//     return res;
// }

// cool_dir *cl_new_dir() {
//     cool_dir *res = malloc(sizeof(cool_dir));

//     struct stat *st = calloc(1, sizeof(struct stat));
//     st->st_uid = 1000;
//     st->st_gid = 1000;
//     st->st_ino = 0;
//     st->st_mode = S_IFDIR | 0444;
//     st->st_size = 0;
//     time_t t = time(NULL);
//     st->st_ctime = t;
//     st->st_atime = t;
//     st->st_mtime = t;

//     res->entries = NULL;
//     res->entry_cnt = 0;
//     res->stat = st;

//     return res;
// }

// cool_freg *cl_new_file(ino_t inode) {
//     cool_freg *res = malloc(sizeof(cool_freg));
//     res->inode = inode;
//     return res;
// }

// Dirent *cl_add_file(cool_dir *dir, char *name, cool_freg *file) {
//     dir->entry_cnt++;
//     size_t new_size = dir->entry_cnt * sizeof(Dirent*);
//     if (dir->entries == NULL) {
//         dir->entries = malloc(new_size);
//     } else {
//         dir->entries = realloc(dir->entries, new_size);
//     }

//     Dirent *new_entry = malloc(sizeof(Dirent));
//     new_entry->type = (int)S_IFREG;
//     new_entry->name = name;
//     new_entry->node.file = file;

//     dir->entries[dir->entry_cnt - 1] = new_entry;

//     return new_entry;
// }

// Dirent *cl_add_dir(cool_dir *dir, char *name, cool_dir *subdir) {
//     dir->entry_cnt++;
//     size_t new_size = dir->entry_cnt * sizeof(Dirent*);
//     if (dir->entries == NULL) {
//         dir->entries = malloc(new_size);
//     } else {
//         dir->entries = realloc(dir->entries, new_size);
//     }

//     Dirent *new_entry = malloc(sizeof(Dirent));
//     new_entry->type = (int)S_IFDIR;
//     new_entry->name = name;
//     new_entry->node.dir = subdir;

//     dir->entries[dir->entry_cnt - 1] = new_entry;

//     return new_entry;
// }

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

// void cl_remove_entry(cool_dir *dir, Dirent *entry) {
//     size_t index = -1;
//     for (size_t i = 0; i < dir->entry_cnt; i++) {
//         if (dir->entries[i] == entry) {
//             index = i;
//             break;
//         }
//     }

//     if (index != -1) {
//         dir->entries[index] = NULL;
//         qsort(dir->entries, dir->entry_cnt, sizeof(Dirent*), push_null_at_end);
//         size_t new_count = dir->entry_cnt - 1;
//         dir->entries = realloc(dir->entries, sizeof(Dirent*) * new_count);
//         dir->entry_cnt = new_count;
        
//         log_debug("[remove_entry] -> OK");
//     }
// }

// Dirent *cl_get_dirent(Dirent *parent, const char *name) {
//     if (parent->type == S_IFDIR) {
//         cool_dir *dir = parent->node.dir;
//         for (size_t i = 0; i < dir->entry_cnt; i++) {
//             Dirent *child = dir->entries[i];
//             if (strcmp(child->name, name) == 0) {
//                 return child;
//             }
//         }
//     }

//     return NULL;
// }

// void cl_print_root(cool_dir* root) {
//     printf("/\n");
//     for (size_t i = 0; i < root->entry_cnt; i++) {
//         cl_print_dirent(root->entries[i], 1);
//     }
// }

// const char *BLUE = "\x1b[94m";
// const char *CLEAR_TERM = "\x1b[0m";

// void cl_print_dirent(Dirent *ent, int level) {
//     const char *name = ent->name;
//     char *indent = malloc(level * 2 + 1);
//     for (size_t i = 0; i < (level * 2); i++) {
//         indent[i] = ' ';
//     }

//     indent[level * 2] = '\0';

//     switch (ent->type) {
//         case S_IFDIR:
//             printf("%s%s%s%s\n", indent, BLUE, name, CLEAR_TERM);
//             break;
//         case S_IFREG:
//             printf("%s%s (%lu)\n", indent, name, ent->node.file->inode);
//             break;
//         default:
//             printf("%s? %s\n", indent, name);
//             break;
//     }

//     if (ent->type == S_IFDIR) {
//         cool_dir *dir = ent->node.dir;
//         for (size_t i = 0; i < dir->entry_cnt; i++) {
//             cl_print_dirent(dir->entries[i], level + 1);
//         }
//     }
// }

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
