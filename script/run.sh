#!/bin/bash

set -e

DIR=$(dirname "$(dirname "$(realpath "$0")")")
cd "$DIR"

if [[ ! -d "$DIR/build" || "$1" ]]; then
  cmake --preset debug -G Ninja
fi

cmake --build --preset debug && ./bin/pong
