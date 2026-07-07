#include "DynArray.hpp"
#include "Map.hpp"
#include "UniquePtr.h"

namespace bmin {

template class DynArray<int>;
template class DynArray<String>;
template class DynArray<const char*>;

template class Map<String, int>;
template class Map<String, String>;

template class UniquePtr<int>;
template class UniquePtr<String>;

}  // namespace bmin
