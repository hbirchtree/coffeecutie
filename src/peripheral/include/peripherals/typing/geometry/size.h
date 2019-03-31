#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>
#include <peripherals/typing/vectors/vectors.h>

namespace typing {
namespace geometry {

using namespace libc_types;
using namespace type_safety;

struct base_size
{
    constexpr base_size()
    {
    }
    constexpr base_size(i32, i32)
    {
    }
    constexpr base_size(i32, i32, i32)
    {
    }
};

template<
    typename T,

    typename is_pod<T>::type* = nullptr

    >
struct size_2d : base_size
{
    template<typename U>
    operator size_2d<U>() const
    {
        return this->convert<U>();
    }

    size_2d(vectors::tvector<T, 2> const& src) : w(src.x()), h(src.y())
    {
    }
    constexpr size_2d(T wd, T hg) : w(wd), h(hg)
    {
    }
    constexpr size_2d() : w(0), h(0)
    {
    }

    T w;
    T h;

    T area() const
    {
        return w * h;
    }

    template<typename U>
    inline size_2d<U> convert() const
    {
        return size_2d<U>(w, h);
    }

    FORCEDINLINE size_2d<T> operator/(const T& d)
    {
        return size_2d<T>(this->w / d, this->h / d);
    }
    FORCEDINLINE size_2d<T> operator*(const T& d)
    {
        return size_2d<T>(this->w * d, this->h * d);
    }

    FORCEDINLINE size_2d<T>& operator/=(const T& d)
    {
        (*this) = (*this) / d;
        return *this;
    }
    FORCEDINLINE size_2d<T>& operator*=(const T& d)
    {
        (*this) = (*this) * d;
        return *this;
    }

    inline T operator[](size_t i) const
    {
        switch(i)
        {
        case 0:
            return w;
        case 1:
            return h;
        default:
            return 0;
        }
    }

    inline T& operator[](size_t i)
    {
        switch(i)
        {
        case 0:
            return w;
        case 1:
            return h;
        }

        return w;
    }
};

template<
    typename T,

    typename is_pod<T>::type* = nullptr

    >
struct size_3d : base_size
{
    size_3d(vectors::tvector<T, 3> const& src) :
        width(src.x()), height(src.y()), depth(src.z())
    {
    }
    size_3d(T w, T h, T d) : width(w), height(h), depth(d)
    {
    }
    size_3d() : width(0), height(0), depth(0)
    {
    }

    template<typename U>
    inline size_3d<U> convert() const
    {
        return size_3d<U>(width, height, depth);
    }

    T width, height, depth;

    T volume() const
    {
        return width * height * depth;
    }

    inline T operator[](size_t i) const
    {
        switch(i)
        {
        case 0:
            return width;
        case 1:
            return height;
        case 2:
            return depth;
        default:
            return 0;
        }
    }

    inline T& operator[](size_t i)
    {
        switch(i)
        {
        case 0:
            return width;
        case 1:
            return height;
        case 2:
            return depth;
        }

        return width;
    }
};


}
}
