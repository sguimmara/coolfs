#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "test.h"

#include "../src/inode.h"
#include "../src/log/log.h"

void mk_inode_returns_correct_stat() {
    cool_inode *inode = mk_inode(2, "foo", "hello");

    assert(inode->st->st_ino == 2);
    assert(inode->st->st_size == 5);
    assert(inode->st->st_mode == (S_IFREG | 0444));

    pass(__FUNCTION__);
}

void mk_inode_returns_correct_name() {
    cool_inode *inode = mk_inode(2, "foo", "hello");

    assert(strcmp("foo", inode->name) == 0);

    pass(__FUNCTION__);
}

void serialize_inode_should_return_correct_values() {
    cool_inode *inode = mk_inode(32, "foo", "bar");
    disk_inode *si = serialize_inode(inode);

    assert(si->st.st_ino == inode->st->st_ino);

    free(si);

    pass(__FUNCTION__);
}

void serialize_inode_section() {
    push_section(__FUNCTION__);

    serialize_inode_should_return_correct_values();

    pop();
}

void mk_inode_section() {
    push_section(__FUNCTION__);

    mk_inode_returns_correct_stat();
    mk_inode_returns_correct_name();

    pop();
}

int main(int argc, char **argv) {
    init_test(__FILE__);

    mk_inode_section();
    serialize_inode_section();

    return 0;
}
