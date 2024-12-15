#!/bin/bash

if [ "$#" -ne 3 ]; then
  echo "Usage: $0 <input-file> <prefix> <output-file>"
  exit 1
fi

input="$1"
prefix="$2"
output="$3"

if [ ! -f "$1" ]; then
  echo "Error: Input file '$1' does not exist."
  exit 2
fi

if [[ -z "$OBJC" ]]; then
  OBJC="objdump"
fi

if [[ -z "$FORMAT" ]]; then
  FORMAT=$("$OBJC" -f /bin/ls 2>/dev/null | grep "file format" | awk '{print $NF}' | xargs)
fi

if [ -z "$FORMAT" ]; then
  echo "Error: Could not detect file format."
  exit 3
fi

c_id="${input//[^a-zA-Z0-9]/_}"

c_name="$(basename "$input")"
c_name="${c_name//[^a-zA-Z0-9]/_}"

"$OBJC" -I binary -O "$FORMAT" "$input" "$output"
if [ $? -ne 0 ]; then
  echo "Error: objcopy failed."
  exit 4
fi

"$OBJC" --redefine-sym "_binary_${c_id}_start=${prefix}_${c_name}_start" "$output"
"$OBJC" --redefine-sym "_binary_${c_id}_end=${prefix}_${c_name}_end" "$output"
"$OBJC" --redefine-sym "_binary_${c_id}_size=${prefix}_${c_name}_size" "$output"
