#ifndef _COOLFS_PATH_H_
#define _COOLFS_PATH_H_

#include <stddef.h>

/**
 * @brief Contains the fragments of a path.
 */
typedef struct PathBuf {
    size_t count; // The number of fragments in the path
    char **fragments;
} PathBuf;

/**
 * @brief Parses the path into a PathBuf struct.
 * If the path is the root directory ("/"), the result has a
 * fragment count of zero, and the function returns 0.
 *
 * @param [in] buf The path to parse.
 * @param [out] result The resulting PathBuf.
 * @return int The status.
 */
int parse_path(const char *buf, PathBuf *result);

/**
 * @brief Returns the parent directory of the path.
 *
 * @return char*
 */
char *parent(const PathBuf* path);

/**
 * @brief Returns the base name of the path.
 *
 */
char *basename(const PathBuf *path);

#endif /* _COOLFS_PATH_H_ */