#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/geometry/size.h>

namespace Coffee {

template<typename T>
using size_2d = typing::geometry::size_2d<T>;

template<typename T>
using size_3d = typing::geometry::size_3d<T>;

using Size  = typing::geometry::size_2d<libc_types::u32>;
using Size3 = typing::geometry::size_3d<libc_types::u32>;
} // namespace Coffee
