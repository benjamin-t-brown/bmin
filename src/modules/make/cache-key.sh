#!/bin/sh

compiler=$1
flags=$2
compiler_path=$(command -v "$compiler" 2>/dev/null || printf '%s' "$compiler")
compiler_version=$("$compiler" --version 2>/dev/null | sed -n '1p')

printf '%s\n%s\n%s\n' "$compiler_path" "$compiler_version" "$flags" |
  cksum | awk '{print $1}'
