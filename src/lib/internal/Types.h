#pragma once

using size_t = decltype(sizeof(0));
using ptrdiff_t = decltype(static_cast<char*>(nullptr) -
                           static_cast<char*>(nullptr));
