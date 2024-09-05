#!/bin/bash

cd $(dirname $(pwd)/$0)

[[ ! -d build/debug ]] && cmake --preset=test -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -G Ninja
cmake --build --preset=debug -j 6 && cd build/debug && ctest --extra-verbose -L CLI -C Debug
