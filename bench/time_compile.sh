#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")" && pwd)"
SIDE="${1:-}"

if [[ "$SIDE" != bmin && "$SIDE" != std ]]; then
  echo "usage: $0 bmin|std" >&2
  exit 1
fi

if [[ "$SIDE" == bmin ]]; then
  make -C "$ROOT/../src" native
fi

DIR="$ROOT/$SIDE"
make -C "$DIR" clean

if command -v /usr/bin/time >/dev/null 2>&1; then
  echo "=== compile $SIDE ==="
  /usr/bin/time -f 'wall %e sec  (user %U sys %S)' make -C "$DIR" all
else
  echo "=== compile $SIDE ==="
  TIMEFORMAT='wall %R sec'
  time make -C "$DIR" all
fi

echo "=== run $SIDE ==="
if [[ -x "$DIR/bench.exe" ]]; then
  "$DIR/bench.exe"
else
  "$DIR/bench"
fi
