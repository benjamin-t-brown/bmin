# bmin

A minimal C++20 container library intended to replace common `std` usage
(`string`, `vector`, `unordered_map`, `unique_ptr`) with simpler
implementations and faster compile times via `extern template` +
pre-instantiation in `libbmin.a`.

## Build

```bash
cd src
make clean
make native
```

### Windows (MSYS2 UCRT64)

From PowerShell or cmd, use the MSYS2 shell so `make`, `g++`, and `ar` are on
PATH:

```bash
C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64 -use-full-path -c "cd /c/progs/bmin/src && make clean && make native && cd ../tests && make test"
```

Adjust the repo path if yours is not `C:/progs/bmin`. Build only:

```bash
C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64 -use-full-path -c "cd /c/progs/bmin/src && make native"
```

Output is installed to `bmin/`:

```
bmin/lib/libbmin.a
bmin/include/*.h
bmin/include/*.hpp
```

## Test

```bash
cd tests
make test
```

Runs unit checks and compile-fail scripts (no external test framework).

## Link in your project

```
-I path/to/bmin/include -L path/to/bmin/lib -lbmin
```

Include individual headers (`String.h`, `DynArray.h`, …). Use `All.h` only in
tests or quick experiments.

## Std → bmin mapping

| std | bmin |
|-----|------|
| `std::string` | `bmin::String` |
| `std::to_string(x)` | `bmin::to_string(x)` |
| `std::vector<T>` | `bmin::DynArray<T>` |
| `std::unordered_map<K,V>` | `bmin::Map<K,V>` |
| `std::unique_ptr<T>` | `bmin::UniquePtr<T>` |
| `std::list<T>` | `bmin::List<T>` |
| `std::queue<T>` | `bmin::Queue<T>` |
| `std::ostringstream` | `bmin::StringStream` |
| `std::string_view` (optional) | `bmin::StringInterop.h` |

## Templates and compile time

`DynArray.h`, `Map.h`, `List.h`, and `Queue.h` include their `.hpp` implementation at the bottom
(like `std::vector`), so any `DynArray<T>` / `Map<K,V>` works from a normal
`#include` — no extra instantiation file in your app.

Common types are also pre-compiled into `libbmin.a` via `extern template` +
`Instantiations.cpp`. For those types your translation units parse the
template but do not emit duplicate object code.

**Custom types** (e.g. `DynArray<UniquePtr<Foo>>`, `Map<Point, int>`):

- Include `DynArray.h` / `Map.h` / `UniquePtr.h` as usual; the compiler
  instantiates in your translation unit automatically.
- `T` must meet the usual container requirements (constructible, movable or
  copyable as needed).
- `Map` keys need `operator==` and a `bmin::Hash<K>`. Trivially-copyable keys
  (POD structs, enums, pointers) use the default byte hash; specialize
  `bmin::Hash<T>` for other key types (same idea as `std::hash`).
- Link `-lbmin` for `String` and the pre-built instantiations.

**Optional:** add a type to `Instantiations.cpp` (and `extern template` in the
header) when you want it compiled only into `libbmin.a` and shared across many
translation units — same idea as extending the library's pre-built set.

## Compile-time benchmark

`bench/` contains two equivalent sample apps (6 translation units each):

| Program | Containers | Link |
|---------|------------|------|
| `bench/bmin` | `bmin::String`, `DynArray`, `Map`, `UniquePtr` | `libbmin.a` |
| `bench/std` | `std::string`, `vector`, `unordered_map`, `unique_ptr` | libc++ only |

Both use `-std=c++20 -O0` so compile time dominates.

```bash
cd bench
make all          # time a clean compile + run for both
make time-bmin    # bmin only
make time-std     # std only
```

Windows (MSYS2):

```bash
C:/msys64/msys2_shell.cmd -defterm -here -no-start -ucrt64 -use-full-path -c "cd /c/progs/bmin/bench && make all"
```

Compare the `wall` times printed for each side. For per-file detail:

```bash
g++ -ftime-report -c worker3.cpp   # in bench/bmin or bench/std
```
