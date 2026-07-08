#include "DynArray.h"  // IWYU pragma: keep
#include "List.h"  // IWYU pragma: keep
#include "Map.h"  // IWYU pragma: keep
#include "Queue.h"  // IWYU pragma: keep
#include "UniquePtr.h"

namespace bmin {

template class DynArray<int>;
template class DynArray<String>;
template class DynArray<const char*>;

template class List<int>;
template class List<String>;

template class Map<String, int>;
template class Map<String, String>;

template class Queue<int>;
template class Queue<String>;

template class UniquePtr<int>;
template class UniquePtr<String>;

}  // namespace bmin
