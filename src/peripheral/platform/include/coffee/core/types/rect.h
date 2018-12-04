#pragma once

#include <peripherals/typing/geometry/rect.h>

namespace Coffee {

using Rect   = typing::geometry::rect<libc_types::u32>;
using Rect64 = typing::geometry::rect<libc_types::u64>;

template<typename T>
using rect = typing::geometry::rect<T>;
} // namespace Coffee
