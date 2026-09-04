# Modules (dual-ship)

Named modules ship **alongside** the classic header library. `make native`
installs both into `bmin/`.

| Consumer | Include / import | Link |
|----------|------------------|------|
| Headers | `#include "String.h"` | `-lbmin` |
| Modules | `import bmin.containers` | `-lbmin_modules` |

Do not mix the two styles for the same types in one program.

## Local commands

```bash
cd src
make native          # headers + modules install
make modules         # smoke test only
cd modules && make time-compile
cd ../../tests && make test  # same unit suite through both APIs
```

Installed consumer notes: `bmin/modules/README.md` (from `MODULES.md`).
Consumer Make helpers: `modules/make/use.mk` + `build-bmi.mk`.

## Module graph

`core` → `dynarray` / `unique_ptr` / `list` → `string` → `hash` / `queue` → `map` → `containers`

`bmin.core` exports the supported `move`, `forward`, and `exchange` utilities.
Its nested `bmin::detail` namespace is reachable support for exported templates,
but is not public API.
