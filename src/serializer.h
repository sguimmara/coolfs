#ifndef _COOLFS_SERIALIZER_H_
#define _COOLFS_SERIALIZER_H_

#include <stdio.h>

#include "block.h"

typedef struct SerializerState {
    FILE* stream;
    size_t block_size;
} SerializerState;

SerializerState* ser_init(FILE* stream, size_t block_size);

void ser_block(const Block *block, SerializerState *st);
Block* deser_block(FILE* stream);

#endif /* _COOLFS_SERIALIZER_H_ */