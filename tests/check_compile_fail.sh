#!/bin/sh
set -e
CXX="${CXX:-g++}"
for f in compile_fail/*.cpp; do
  if "$CXX" -std=c++23 -I../bmin/include -c "$f" -o /dev/null 2>/dev/null; then
    echo "expected compile failure: $f"
    exit 1
  fi
done
echo "compile_fail: OK"
