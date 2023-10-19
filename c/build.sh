#!/usr/bin/env sh

# gcc -static -O0 -ggdb demo.c -o demo  # debug build
gcc -static -march=native -Werror -O3 -DNDEBUG demo.c -o demo  # release build
