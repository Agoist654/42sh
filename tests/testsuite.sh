#!/bin/sh

REF_OUT="tests/ref.out"
TEST_OUT="tests/test.out"

test() {
    echo 
    echo 
    diff $GOT $EXP

}
