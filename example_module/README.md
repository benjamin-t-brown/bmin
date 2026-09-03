# module_example — word count with bmin modules

Standalone demo: read a text file via `std::filesystem`, count words with
`bmin::String`, `DynArray`, `Map`, and `UniquePtr` via `import`.

See `../example` for the same program using headers (`#include` + `libbmin.a`).

## Build / run

```bash
cd module_example
make clean
make run          # builds bmin if needed, runs on sample.txt
./wordcount other.txt
```

The Makefile includes `modules/make/use.mk` — BMIs and `libbmin_modules.a`
are handled for you (no per-module compile rules).

## Requires

- GCC with `-fmodules-ts`
- Repo `make native` (or let this Makefile trigger it)
