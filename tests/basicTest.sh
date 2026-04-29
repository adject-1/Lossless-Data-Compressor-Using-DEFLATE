#!/bin/bash

rm -f ./test.cmp output.txt

make clean
make

./bin/compressor compress ./samples/sourcecode.txt ./build/test.cmp
