#include <stdlib.h>

#include "serializer.h"

SerializerState*
ser_init(FILE *stream, size_t block_size) {
    SerializerState* res = malloc(sizeof(SerializerState));

    res->block_size = block_size;
    res->stream = stream;

    return res;
}

void ser_block(const Block *block, SerializerState *st) {
    fwrite(&block->size, sizeof(uint16_t), 1, st->stream);
    fwrite(block->content, block->size, 1, st->stream);
    if (block->size < st->block_size) {
        fseek(st->stream, st->block_size - block->size - sizeof(uint16_t), SEEK_CUR);
    }
}

Block *deser_block(FILE *stream) { return NULL; }
