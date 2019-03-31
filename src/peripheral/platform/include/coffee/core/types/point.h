#pragma once

#include <peripherals/typing/geometry/point.h>

namespace Coffee {

using Point  = typing::geometry::point_2d<libc_types::i32>;
using Point3 = typing::geometry::point_3d<libc_types::i32>;

using PtF = typing::geometry::point_2d<libc_types::scalar>;

template<typename T>
using point_2d = typing::geometry::point_2d<T>;
template<typename T>
using point_3d = typing::geometry::point_3d<T>;
} // namespace Coffee
