# bmin

A minimal C++23 container library intended to replace common `std` usage
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
bmin/lib/libbmin_modules.a
bmin/include/*.h
bmin/include/internal/*
bmin/modules/*.cppm          # named modules (dual-ship)
bmin/modules/README.md
```

## Test

```bash
cd tests
make test
```

Runs the same unit suite twice: once through the classic headers and
`libbmin.a`, then through the named modules and `libbmin_modules.a`. It also
compiles every supported module as a direct import and runs compile-fail checks
against both APIs.

## Link in your project

### Headers (`#include`) — default / widest compatibility

```text
-iquote path/to/bmin/include -L path/to/bmin/lib -lbmin
```

Include individual headers (`String.h`, `DynArray.h`, …). Use `All.h` only in
tests or quick experiments. Use `-iquote`, rather than `-I`, because the flat
classic API contains `String.h`; on case-insensitive filesystems it can
otherwise shadow the platform's `<string.h>`.

### Modules (`import`) — GCC with `-fmodules-ts`

Link `-lbmin_modules`. Include the helper Make fragment so you do not list
every module by hand:

```makefile
include path/to/bmin/modules/make/use.mk
main.o: main.cpp | bmin-bmi
	$(CXX) $(BMIN_CXXFLAGS) -c main.cpp -o $@
```

See `bmin/modules/README.md`.

The jointly shipped bmin + SDL2W module toolchain currently targets GCC 15.
On macOS the Make helpers select `g++-15` by default because `/usr/bin/g++` is
Apple Clang. Module-interface compilation explicitly passes `-x c++`.

**Do not mix** `#include` of bmin headers and `import` of bmin modules for the
same types in one program — dual-ship means two parallel APIs, not one ODR.

### Editor (Cursor / VS Code)

Use the official **clangd** extension rather than Microsoft C/C++ IntelliSense.
Generate a compilation database covering the classic library, named modules,
tests, and both examples:

```bash
./compile-commands-module.sh
```

The script prefers upstream Homebrew LLVM on macOS. Install it with
`brew install llvm`, then set the editor's machine-local `clangd.path` to
`$(brew --prefix llvm)/bin/clangd`. The repo enables
`--experimental-modules-support`; restart clangd after regenerating the database.

Set `CLANGXX=/path/to/clang++` when using another driver, and add that exact
executable to clangd's `--query-driver` allowlist.

For TextMate highlighting of `import` / `module`, prefer **Minimal C++ Syntax**
(`daiyousei-qz.minimal-cpp-syntax`) over Better C++ Syntax — the latter often
leaves module keywords uncolored when clangd owns semantic highlighting.
Cursor’s marketplace does not ship that extension, so install from the VS
Marketplace VSIX:

```bash
curl -L -o minimal-cpp-syntax.vsix \
  "https://daiyousei-qz.gallerycdn.vsassets.io/extensions/daiyousei-qz/minimal-cpp-syntax/0.0.4/1649824472376/Microsoft.VisualStudio.Services.VSIXPackage"
cursor --install-extension ./minimal-cpp-syntax.vsix
```

Disable Better C++ Syntax afterward and reload the window.

## Std → bmin mapping

| std | bmin |
|-----|------|
| `std::string` | `bmin::String` |
| `std::to_string(x)` | `bmin::toString(x)` |
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
- `Map` keys need `operator==` and a `bmin::Hash<K>`. Integral, enum, pointer,
  and `String` keys are supported directly. Custom structures must specialize
  `bmin::Hash<T>` so hashing follows their logical equality and never depends
  on object padding (the same idea as specializing `std::hash`).
- Link `-lbmin` for `String` and the pre-built instantiations.

```cpp
struct Point {
  int x;
  int y;

  bool operator==(const Point& other) const {
    return x == other.x && y == other.y;
  }
};

template <>
struct bmin::Hash<Point> {
  size_t operator()(const Point& point) const {
    return bmin::Hash<int>{}(point.x) ^
           (bmin::Hash<int>{}(point.y) + 0x9e3779b9u);
  }
};
```

**Optional:** add a type to `Instantiations.cpp` (and `extern template` in the
header) when you want it compiled only into `libbmin.a` and shared across many
translation units — same idea as extending the library's pre-built set.

## Compile-time benchmark

`bench/` contains two equivalent sample apps (6 translation units each):

| Program | Containers | Link |
|---------|------------|------|
| `bench/bmin` | `bmin::String`, `DynArray`, `Map`, `UniquePtr` | `libbmin.a` |
| `bench/std` | `std::string`, `vector`, `unordered_map`, `unique_ptr` | libc++ only |

Both use `-std=c++23 -O0` so compile time dominates.

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
