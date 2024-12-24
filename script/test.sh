#!/bin/bash

SCRIPT_DIR=$(dirname "$(realpath "$0")")
cd "$(dirname "$SCRIPT_DIR")" || exit

if [[ ! -d "$SCRIPT_DIR/build" || "$1" ]]; then
  cmake --preset debug || exit
fi

cmake --build --preset debug -- -j "$(nproc)" && ./bin/game
