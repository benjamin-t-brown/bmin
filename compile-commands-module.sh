#!/bin/bash
# Generate / merge compile_commands.json entries for bmin C++ modules (clangd).
#
# Usage:
#   ./compile-commands-module.sh
#
# Prefers clang++ (set CLANGXX to override). clangd builds its own BMIs when
# started with --experimental-modules-support; this DB must list every .cppm
# and every TU that imports them.

set -euo pipefail

cd "$(dirname "$0")"
ROOT="$(pwd)"

if [[ -z "${CLANGXX:-}" ]]; then
	if command -v clang++ >/dev/null 2>&1; then
		CLANGXX="$(command -v clang++)"
	elif [[ -x /c/progs/msys2/ucrt64/bin/clang++.exe ]]; then
		CLANGXX=/c/progs/msys2/ucrt64/bin/clang++.exe
	elif [[ -x /ucrt64/bin/clang++.exe ]]; then
		CLANGXX=/ucrt64/bin/clang++.exe
	elif [[ -x "/c/Program Files/LLVM/bin/clang++.exe" ]]; then
		CLANGXX="/c/Program Files/LLVM/bin/clang++.exe"
	else
		# clangd still parses as Clang; a missing binary only hurts system-include
		# discovery. Install LLVM/MSYS2 clang or set CLANGXX for best results.
		CLANGXX=clang++
		echo "warning: clang++ not found; using 'clang++' as the driver name" >&2
	fi
fi

COMPILER="$CLANGXX"
if command -v cygpath >/dev/null 2>&1; then
	COMPILER="$(cygpath -m "$CLANGXX")"
else
	COMPILER="${COMPILER//\\//}"
fi

python - "$ROOT" "$COMPILER" <<'PY'
import json
import sys
from pathlib import Path

root = Path(sys.argv[1]).resolve()
compiler = sys.argv[2].replace("\\", "/")
mod_dir = root / "src" / "modules"
tests_dir = root / "tests"
example_dir = root / "module_example"

# Interface units clangd must see (named modules).
IFACES = [
    "bmin.core.cppm",
    "bmin.dynarray.cppm",
    "bmin.unique_ptr.cppm",
    "bmin.string.cppm",
    "bmin.list.cppm",
    "bmin.queue.cppm",
    "bmin.hash.cppm",
    "bmin.map.cppm",
    "bmin.stringstream.cppm",
    "bmin.string_interop.cppm",
    "bmin.containers.cppm",
]

IMPLS = [
    "bmin.core.cpp",
    "bmin.string.cpp",
    "bmin.stringstream.cpp",
    "bmin.string_interop.cpp",
]

COMMON = [
    "-Wall",
    "-std=c++23",
    "-fPIC",
    f"-I{mod_dir.as_posix()}",
]


def entry(directory: Path, source: Path, extra_args=None) -> dict:
    args = [compiler, *COMMON]
    if extra_args:
        args.extend(extra_args)
    # Hint that this is a module / C++ source for clangd.
    if source.suffix == ".cppm":
        args.extend(["-x", "c++-module"])
    args.extend(["-c", source.as_posix(), "-o", (directory / (source.stem + ".o")).as_posix()])
    return {
        "directory": directory.as_posix(),
        "arguments": args,
        "file": source.as_posix(),
    }


db = []

for name in IFACES:
    src = mod_dir / name
    if src.is_file():
        db.append(entry(mod_dir, src))

for name in IMPLS:
    src = mod_dir / name
    if src.is_file():
        db.append(entry(mod_dir, src))

smoke = mod_dir / "smoke.cpp"
if smoke.is_file():
    db.append(entry(mod_dir, smoke))

if example_dir.is_dir():
    main = example_dir / "main.cpp"
    if main.is_file():
        db.append(entry(example_dir, main))

if tests_dir.is_dir():
    for src in sorted(tests_dir.glob("test_*.cpp")):
        # Header-only harness + modules consumers.
        db.append(entry(tests_dir, src, extra_args=[f"-I{tests_dir.as_posix()}"]))
    main = tests_dir / "test_main.cpp"
    if main.is_file():
        db.append(entry(tests_dir, main, extra_args=[f"-I{tests_dir.as_posix()}"]))
    for src in sorted((tests_dir / "direct_import").glob("*.cpp")):
        db.append(entry(tests_dir, src, extra_args=[f"-I{tests_dir.as_posix()}"]))

out = root / "compile_commands.json"
existing = []
if out.is_file():
    try:
        existing = json.loads(out.read_text(encoding="utf-8"))
    except json.JSONDecodeError:
        existing = []

# Drop previous entries for the same files, then append module DB.
module_files = {e["file"].replace("\\", "/") for e in db}
merged = [
    e
    for e in existing
    if e.get("file", "").replace("\\", "/") not in module_files
]
merged.extend(db)

out.write_text(json.dumps(merged, indent=1) + "\n", encoding="utf-8")
print(f"Wrote {out} ({len(merged)} entries, {len(db)} module-related)")
print(f"Compiler driver: {compiler}")
print("Restart clangd after running this (Command Palette: clangd: Restart language server).")
PY
