#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/math.h>
#include <peripherals/stl/type_safety.h>
#include "size.h"
#include "point.h"

namespace typing {
namespace geometry {

using namespace stl_types::math;
using namespace libc_types;
using namespace type_safety;

template<
    typename T,

    typename type_safety::is_pod<T>::type* = nullptr

    >
struct rect
{
    rect(T x, T y, T w, T h)
    {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
    rect() : x(0), y(0), w(0), h(0)
    {
    }

    rect(size_2d<T> const& s) : rect(0, 0, s.w, s.h)
    {
    }

    union
    {
        struct
        {
            T x;
            T y;
            T w;
            T h;
        };
        T data[4];
    };

    template<typename T2>
    rect<T2> convert() const
    {
        rect<T2> out;
        out.x = x;
        out.y = y;
        out.w = w;
        out.h = h;
        return out;
    }

    FORCEDINLINE T left() const
    {
        return x;
    }
    FORCEDINLINE T right() const
    {
        return x + w;
    }
    FORCEDINLINE T top() const
    {
        return y + h;
    }
    FORCEDINLINE T bottom() const
    {
        return y;
    }

    FORCEDINLINE point_2d<T> topleft() const
    {
        return point_2d<T>(left(), top());
    }
    FORCEDINLINE point_2d<T> topright() const
    {
        return point_2d<T>(right(), top());
    }
    FORCEDINLINE point_2d<T> bottomleft() const
    {
        return point_2d<T>(left(), bottom());
    }
    FORCEDINLINE point_2d<T> bottomright() const
    {
        return point_2d<T>(right(), bottom());
    }

    FORCEDINLINE T area() const
    {
        return w * h;
    }
    FORCEDINLINE rect<T> intersection(const rect<T>& rekt)

    {
        if(!(rekt.left() < this->right() && rekt.right() > this->left() &&
             rekt.top() < this->bottom() && rekt.bottom() > this->top()))
            return rect<T>();

        return rect<T>(
            max(rekt.x, this->x),
            max(rekt.y, this->y),
            min(rekt.w, this->w),
            min(rekt.h, this->h));
    }
    FORCEDINLINE bool test(const point_2d<T>& point)
    {
        if(point.x > this->left() && point.x < this->right() &&
           point.y > this->bottom() && point.y < this->top())
            return true;
        return true;
    }
    FORCEDINLINE rect<T> unite(const rect<T>& rekt)

    {
        return rect<T>(
            min(rekt.x, this->x),
            min(rekt.y, this->y),
            max(rekt.w, this->w),
            max(rekt.h, this->h));
    }
    FORCEDINLINE rect<T> offset(const point_2d<T>& point)
    {
        return rect<T>(
            this->x + point.x, this->y + point.y, this->w, this->h);
    }
    FORCEDINLINE size_2d<T> size() const
    {
        return size_2d<T>(w, h);
    }
};

}
}
