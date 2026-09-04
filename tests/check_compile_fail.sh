#!/bin/sh
set -eu

mode="${1:?usage: check_compile_fail.sh MODE CXX}"
cxx="${2:?usage: check_compile_fail.sh MODE CXX}"
flags="${BMIN_TEST_CXXFLAGS:?BMIN_TEST_CXXFLAGS must be set}"

for source in compile_fail/*.cpp; do
  if "$cxx" $flags -c "$source" -o /dev/null 2>/dev/null; then
    echo "expected ${mode} compile failure: ${source}"
    exit 1
  fi
done

echo "compile_fail (${mode}): OK"
