#!/bin/bash

cd "$(dirname "$0")"

export CC=/ucrt64/bin/gcc.exe
export CXX=/ucrt64/bin/g++.exe
COMPILER="C:/msys64/ucrt64/bin/g++.exe"

compiledb make -C src all
mv -f src/compile_commands.json compile_commands.json 2>/dev/null || true

python - <<'PY'
import json
from pathlib import Path

compiler = "C:/msys64/ucrt64/bin/g++.exe"
path = Path("compile_commands.json")
db = json.loads(path.read_text(encoding="utf-8"))

for entry in db:
    args = entry.get("arguments", [])
    if not args or not args[0].replace("\\", "/").endswith("g++.exe"):
        entry["arguments"] = [compiler] + args

src_root = Path("src")


def header_entry(template, header_rel):
    args = list(template["arguments"])
    header_rel = str(header_rel).replace("\\", "/")
    for i, arg in enumerate(args):
        if arg == "-c" and i + 1 < len(args):
            args[i + 1] = header_rel
            break
    if "-x" not in args:
        c_idx = args.index("-c")
        args.insert(c_idx, "c++-header")
        args.insert(c_idx, "-x")
    return {
        "directory": template["directory"],
        "arguments": args,
        "file": header_rel,
    }


# Mirror .cpp compile commands for sibling headers.
extra = []
known = {e["file"].replace("\\", "/") for e in db}
for entry in db:
    cpp = Path(entry["file"])
    if cpp.suffix != ".cpp":
        continue
    for ext in (".h", ".hpp"):
        header = cpp.with_suffix(ext)
        if not (src_root / header).exists():
            continue
        rel = str(header).replace("\\", "/")
        if rel in known:
            continue
        extra.append(header_entry(entry, header))
        known.add(rel)

# Headers without a matching .cpp (e.g. Map.h, DynArray.hpp).
template = next(e for e in db if e["file"].endswith("Instantiations.cpp"))
for pattern in ("lib/*.h", "lib/*.hpp", "lib/internal/*.h"):
    for header in sorted(src_root.glob(pattern)):
        rel = str(header.relative_to(src_root)).replace("\\", "/")
        if rel in known:
            continue
        extra.append(header_entry(template, rel))
        known.add(rel)

db.extend(extra)
path.write_text(json.dumps(db, indent=1) + "\n", encoding="utf-8")
PY
