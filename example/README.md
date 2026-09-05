# example — word count with bmin headers

Same demo as `../example_module`, but using the classic `#include` API and
`libbmin.a` (no C++ modules / BMIs).

## Build / run

```bash
cd example
make clean
make run          # builds bmin if needed, runs on sample.txt
./wordcount other.txt
```

## Requires

- C++23 compiler
- Repo `make native` (or let this Makefile trigger it)
