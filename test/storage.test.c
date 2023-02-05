#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#include "../src/storage.h"

void size_should_match() {
    char *data = "abc";
    size_t size = strlen(data);
    block *block = make_block(data, size);

    assert(size == block->size);

    pass(__FUNCTION__);
}

void content_should_match() {
    char *data = "abc";
    size_t size = strlen(data);
    block *block = make_block(data, size + 1);

    assert(strcmp(data, block->content) == 0);

    pass(__FUNCTION__);
}

void returns_null_if_block_size_greater_than_BLOCK_SIZE() {
    block *block = make_block((const char *)"", BLOCK_SIZE + 1);
    assert(block == NULL);

    pass(__FUNCTION__);
}

void mk_block_section() {
    push_section(__FUNCTION__);

    content_should_match();
    size_should_match();
    returns_null_if_block_size_greater_than_BLOCK_SIZE();

    pop();
}

void read_blk_should_return_NULL_if_block_does_not_exist() {
    assert(get_block(1) == NULL);

    pass(__FUNCTION__);
}

void write_blk_should_write_the_block() {
    const char *data1 = "abc";
    const size_t size1 = strlen(data1) + 1;

    const char *data2 = "def";
    const size_t size2 = strlen(data2) + 1;

    block *blk1 = make_block(data1, size1);
    block *blk2 = make_block(data2, size2);

    block_write(blk1);
    block_write(blk2);

    block *r1 = get_block(blk1->no);
    block *r2 = get_block(blk2->no);

    assert(r1 != NULL);
    assert(r2 != NULL);

    assert(strcmp(r1->content, data1) == 0);
    assert(strcmp(r2->content, data2) == 0);
    assert(r1->size == size1);
    assert(r2->size == size2);

    pass(__FUNCTION__);
}

void read_blk_section() {
    push_section(__FUNCTION__);

    read_blk_should_return_NULL_if_block_does_not_exist();

    pop();
}

void write_blk_section() {
    push_section(__FUNCTION__);

    write_blk_should_write_the_block();

    pop();
}

int main(int argc, char **argv) {
    init_test(__FILE__);

    mk_block_section();
    read_blk_section();
    write_blk_section();

    return 0;
}
