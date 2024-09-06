#!/bin/bash

cd $(dirname $(pwd)/$0)

[[ ! -d build/debug ]] && cmake --preset=test -G Ninja -D CMAKE_C_COMPILER=clang -D CMAKE_CXX_COMPILER=clang++
cmake --build --preset=debug -j 6 && cd build/debug && ctest --extra-verbose -L CLI -C Debug
