#!/bin/bash
set -e

DIR=$(dirname "$(realpath "$0")")
cd "$DIR"

if [[ ! -d "$DIR/build" || "$1" ]]; then
  cmake --preset release -G Ninja
fi

cmake --build --preset release

mkdir "out"
cd out

cp "../bin/pong" "."
cp "../assets" "." -r
