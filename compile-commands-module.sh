#!/bin/bash
# Generate compile_commands.json entries for both bmin APIs (clangd).
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

if [[ -z "${CLANGXX:-}" && "$(uname -s)" == Darwin ]]; then
	for candidate in /opt/homebrew/opt/llvm/bin/clang++ /usr/local/opt/llvm/bin/clang++; do
		if [[ -x "$candidate" ]]; then
			CLANGXX="$candidate"
			break
		fi
	done
fi

if [[ -z "${CLANGXX:-}" ]]; then
	if [[ -x /c/progs/msys2/ucrt64/bin/clang++.exe ]]; then
		CLANGXX=/c/progs/msys2/ucrt64/bin/clang++.exe
	elif [[ -x /ucrt64/bin/clang++.exe ]]; then
		CLANGXX=/ucrt64/bin/clang++.exe
	elif [[ -x "/c/Program Files/LLVM/bin/clang++.exe" ]]; then
		CLANGXX="/c/Program Files/LLVM/bin/clang++.exe"
	elif command -v clang++ >/dev/null 2>&1; then
		CLANGXX="$(command -v clang++)"
	else
		# clangd still parses as Clang; a missing binary only hurts system-include
		# discovery. Install LLVM/MSYS2 clang or set CLANGXX for best results.
		CLANGXX=clang++
		echo "warning: clang++ not found; using 'clang++' as the driver name" >&2
	fi
fi

if [[ -z "${PYTHON:-}" || ! -x "${PYTHON}" ]]; then
	if command -v python >/dev/null 2>&1; then
		PYTHON=python
	elif command -v python3 >/dev/null 2>&1; then
		PYTHON=python3
	else
		echo "python not found on PATH" >&2
		exit 1
	fi
fi

COMPILER="$CLANGXX"
if command -v cygpath >/dev/null 2>&1; then
	COMPILER="$(cygpath -m "$CLANGXX")"
else
	COMPILER="${COMPILER//\\//}"
fi

"$PYTHON" - "$ROOT" "$COMPILER" <<'PY'
import json
import sys
from pathlib import Path

root = Path(sys.argv[1]).resolve()
compiler = sys.argv[2].replace("\\", "/")
mod_dir = root / "src" / "modules"
src_dir = root / "src"
classic_dir = src_dir / "lib"
tests_dir = root / "tests"
header_example_dir = root / "example"
module_example_dir = root / "example_module"

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

MODULE_FLAGS = [
    "-Wall",
    "-std=c++23",
    "-fPIC",
    f"-I{mod_dir.as_posix()}",
]

CLASSIC_FLAGS = [
    "-Wall",
    "-std=c++23",
    "-fPIC",
    "-iquote",
    classic_dir.as_posix(),
    "-iquote",
    src_dir.as_posix(),
]


def entry(directory: Path, source: Path, flags, extra_args=None) -> dict:
    args = [compiler, *flags]
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

for src in sorted(classic_dir.rglob("*.cpp")):
    db.append(entry(src_dir, src, CLASSIC_FLAGS))

for name in IFACES:
    src = mod_dir / name
    if src.is_file():
        db.append(entry(mod_dir, src, MODULE_FLAGS))

for name in IMPLS:
    src = mod_dir / name
    if src.is_file():
        db.append(entry(mod_dir, src, MODULE_FLAGS))

smoke = mod_dir / "smoke.cpp"
if smoke.is_file():
    db.append(entry(mod_dir, smoke, MODULE_FLAGS))

if header_example_dir.is_dir():
    main = header_example_dir / "main.cpp"
    if main.is_file():
        db.append(entry(header_example_dir, main, CLASSIC_FLAGS))

main = module_example_dir / "main.cpp"
if main.is_file():
    db.append(entry(module_example_dir, main, MODULE_FLAGS))

if tests_dir.is_dir():
    for src in sorted(tests_dir.glob("test_*.cpp")):
        db.append(entry(
            tests_dir,
            src,
            MODULE_FLAGS,
            [f"-I{tests_dir.as_posix()}", "-DBMIN_TEST_MODULES=1"],
        ))
    for src in sorted((tests_dir / "direct_import").glob("*.cpp")):
        db.append(entry(
            tests_dir,
            src,
            MODULE_FLAGS,
            [f"-I{tests_dir.as_posix()}"],
        ))

out = root / "compile_commands.json"
out.write_text(json.dumps(db, indent=1) + "\n", encoding="utf-8")
print(f"Wrote {out} ({len(db)} entries)")
print(f"Compiler driver: {compiler}")
print("Restart clangd after running this (Command Palette: clangd: Restart language server).")
PY
