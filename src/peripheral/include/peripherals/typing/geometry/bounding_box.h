#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/type_safety.h>

namespace typing {
namespace geometry {

using namespace libc_types;
using namespace type_safety;

template<
    typename T,

    typename type_safety::is_pod<T>::type* = nullptr

    >
struct boundingbox
{
    boundingbox(T w, T h, T d, bool centered = false)
    {
        if(centered)
        {
            this->w = w / T(2);
            this->h = h / T(2);
            this->d = d / T(2);

            this->x = -w / T(2);
            this->y = -h / T(2);
            this->z = -d / T(2);
        } else
        {
            this->w = w;
            this->h = h;
            this->d = d;
        }
    }

    boundingbox(T x, T y, T z, T w, T h, T d)
        : x(x)
        , y(y)
        , z(z)
        , w(w)
        , h(h)
        , d(d)
    {
    }

    T x, y, z;
    T w, h, d;

    T x1() const
    {
        return x;
    }

    T x2() const
    {
        return x + w;
    }

    T y1() const
    {
        return y;
    }

    T y2() const
    {
        return y + h;
    }

    T z1() const
    {
        return z;
    }

    T z2() const
    {
        return z + d;
    }
};

} // namespace geometry
} // namespace typing
