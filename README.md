# coolfs

A toy FUSE filesystem.

## Getting started

You will need autotool to generate the Makefiles.

To compile, you need `libfuse` installed. On debian-based distributions:

```shell
sudo apt install -y libfuse-dev
```

Then compile with `make`:

```shell
./configure
make
```

Execute tests with:

```shell
make check
```

## Design

### FUSE layer (`fs.h`)

The FUSE implementation.

```c
int _mkdir(struct inode *parent, const char *name);
int _read(struct inode *inode, char *buf, size_t size, off_t offset);
int _getattr(struct inode* inode, struct stat *st);
```

### Path parser (`path.h`)

Parses paths, split them into fragments, etc.

```c
struct Path {
    size_t fragments;
    char **fragbuf;
};

int parse(const char* path, struct Path *result);
char *dirname(const struct Path *path);
char *filename(const struct Path *path);
```

### Encryption layer (`crypt.h`)

Use AES (provided by OpenSSL) to encrypt and decrypt file content. The initialization vector is randomly generated at inode creation and stored in the inode itself.

```c
#define IV_SIZE 16

/**
 * Generates a random initialization vector.
 */
int create_iv(char *dst);

/**
 * Encrypts the buffer in place. Note: the buffer size must be a
 * multiple of the AES block size (16 byte).
 */ 
int encrypt(const char *iv, const char* key, char *buf, size_t bufsize);

/**
 * Decrypts the buffer in place. Note: the buffer size must be a
 * multiple of the AES block size (16 byte).
 */ 
int decrypt(const char *iv, const char *key, char *buf, size_t bufsize);
```

### Persistence layer (`storage.h`)

```c
int write_blocks(
    blno_t *blocks,
    size_t count,
    const char *buf,
    size_t bufsize);

int read_blocks(
    blno_t *blocks,
    size_t count,
    const char *buf,
    size_t bufsize);
```
