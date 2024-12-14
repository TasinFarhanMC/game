#!/bin/bash

SCRIPT_DIR="$(dirname $(realpath $0))"
cd $(dirname $SCRIPT_DIR)

if [[ ! -d "$SCRIPT_DIR/build" || "$1" ]]; then
  cmake --preset debug

  if [ $? -ne 0 ]; then
    exit $?
  fi
fi

cmake --build --preset debug && ./bin/pong
