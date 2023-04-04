#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>
#include <peripherals/typing/vectors/vectors.h>

namespace typing {
namespace geometry {

using namespace libc_types;
using namespace type_safety;

struct base_point
{
    base_point()
    {
    }
    base_point(i32, i32)
    {
    }
    base_point(i32, i32, i32)
    {
    }
};

template<
    typename T,

    typename is_pod<T>::type* = nullptr

    >
struct point_2d : base_point
{
    using value_type = T;
    static constexpr size_t row_size = 2;

    point_2d(vectors::tvector<T, 2> const& src) : x(src.x()), y(src.y())
    {
    }
    point_2d(T x, T y) : x(x), y(y)
    {
    }
    point_2d()
    {
    }

    T x, y;

    FORCEDINLINE T distance(const point_2d<T>& p)
    {
        return std::sqrt(
            std::pow(this->x - p.x, 2) + std::pow(this->y - p.y, 2));
    }

    template<typename T2>
    point_2d<T2> convert() const
    {
        point_2d<T2> out;
        out.x = x;
        out.y = y;
        return out;
    }

    template<typename T2>
    vectors::tvector<T2, 2> toVector() const
    {
        return {static_cast<T2>(x), static_cast<T2>(y)};
    }
};

template<
    typename T,

    typename is_pod<T>::type* = nullptr

    >
struct point_3d : base_point
{
    using value_type = T;
    static constexpr size_t row_size = 3;

    point_3d(vectors::tvector<T, 3> const& src) :
        x(src.x()), y(src.y()), z(src.z())
    {
    }
    point_3d(T x, T y, T z) : x(x), y(y), z(z)
    {
    }
    point_3d() : x(0), y(0), z(0)
    {
    }

    template<typename U>
    point_3d<U> convert() const
    {
        return point_3d<U>(x, y, z);
    }

    template<typename T2>
    vectors::tvector<T2, 3> toVector() const
    {
        return {x, y, z};
    }

    T x, y, z;
};


}
}
