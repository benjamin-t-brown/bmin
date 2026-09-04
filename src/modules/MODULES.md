# Using bmin C++ modules (dual-ship)

This tree is installed next to the classic headers. Pick **one** style per
program — do not `#include` bmin headers and `import` bmin modules for the
same types in one binary (they are parallel APIs, not the same entities).
Both products are built and tested; neither is a compatibility wrapper around
the other.

## Layout (after `make native`)

```
bmin/lib/libbmin.a            # header / #include consumers
bmin/lib/libbmin_modules.a    # import consumers (object code)
bmin/include/…                # classic headers
bmin/modules/*.cppm           # module interfaces
bmin/modules/make/use.mk      # consumer Make helper
bmin/modules/make/build-bmi.mk
```

## Header path (unchanged)

```text
-iquote bmin/include -L bmin/lib -lbmin
#include "String.h"
```

Quote-only lookup avoids `String.h` shadowing the platform's `<string.h>` on
case-insensitive filesystems.

## Module path (easy)

`libbmin_modules.a` is the link library. You still need BMIs at compile time;
those are **not** portable across GCC versions, so they are generated locally.

```makefile
include path/to/bmin/modules/make/use.mk

main.o: main.cpp bmin-bmi
	$(CXX) $(BMIN_CXXFLAGS) -c main.cpp -o $@

app: main.o
	$(CXX) $(BMIN_CXXFLAGS) -o $@ main.o $(BMIN_LDLIBS)
```

`bmin-bmi` builds every module interface into `./gcm.cache` once. You do **not**
need to list individual `.cppm` files in your Makefile.

```cpp
import bmin.containers;

int main() {
  bmin::String s("hi");
  bmin::Map<bmin::String, int> m;
  m.insert(s, 1);
  return 0;
}
```

Prefer `import bmin.containers` unless you need a smaller surface.
Import `bmin.string_interop` separately for `std::string_view` helpers.

`bmin.core` contains the supported `bmin::move`, `bmin::forward`, and
`bmin::exchange` utilities required by the containers. Names nested under
`bmin::detail` are reachable so exported templates can instantiate, but they
are not public API.

## Compiler contract

The native module build uses GCC 15 for the jointly shipped bmin + SDL2W
toolchain. Override `CXX`, `BMIN_MODULE_CXXFLAGS`, or
`BMIN_MODULE_INTERFACE_FLAGS` when integrating another supported compiler.
The `.cppm` rules pass `-x c++` explicitly because GCC does not infer the
language from that suffix consistently on every platform.

## clangd / editor (modules)

```bash
./compile-commands-module.sh
```

Then restart clangd. Repo settings enable `--experimental-modules-support`.

For `import` / `module` keyword coloring in Cursor, install **Minimal C++
Syntax** from a VSIX (not in Cursor’s marketplace) and disable Better C++
Syntax — see the main `README.md` “Editor” section.

## Coverage

Modules today: String, DynArray, List, Queue, Map, Hash, UniquePtr,
StringStream, StringInterop, plus the core template-support module.
