#include "bench_workers.h"

#include "DynArray.h"
#include "UniquePtr.h"

void bench_worker4() {
  bmin::UniquePtr<int> value(bmin::makeUnique<int>(42));
  bmin::DynArray<bmin::UniquePtr<int>> owned;
  for (int i = 0; i < 16; ++i) {
    owned.pushBack(bmin::makeUnique<int>(i));
  }
  value.reset(new int(99));
  (void)value.get();
}
