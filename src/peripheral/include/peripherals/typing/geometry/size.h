#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/types.h>

namespace typing {
namespace geometry {

using namespace libc_types;
using namespace type_safety;

template<typename T>
requires std::is_trivial<T>::value struct size_2d
{
    using value_type = T;
    static constexpr size_t row_size = 2;

    static constexpr size_t length() { return 2; }

    template<typename Self, typename U>
    STATICINLINE Self from_values(U w, U h)
    {
        return Self{static_cast<T>(w), static_cast<T>(h)};
    }

    T w{0}, h{0};

    inline T area() const
    {
        return w * h;
    }

    inline T volume() const
    {
        return area();
    }

    inline f32 aspect() const
    {
        return f32(w) / f32(h);
    }

    template<typename U>
    inline operator size_2d<U>() const
    {
        return this->convert<U>();
    }

    template<typename U>
    inline size_2d<U> convert() const
    {
        return size_2d<U>{static_cast<U>(w), static_cast<U>(h)};
    }

    inline size_2d<T> operator/(const T& d)
    {
        return size_2d<T>(this->w / d, this->h / d);
    }
    inline size_2d<T> operator*(const T& d)
    {
        return size_2d<T>(this->w * d, this->h * d);
    }

    inline size_2d<T>& operator/=(const T& d)
    {
        (*this) = (*this) / d;
        return *this;
    }
    inline size_2d<T>& operator*=(const T& d)
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

template<typename T>
requires std::is_trivial<T>::value struct size_3d
{
    using value_type = T;
    static constexpr size_t row_size = 3;

    static constexpr size_t length() { return 2; }

    template<typename U>
    inline size_3d<U> convert() const
    {
        return size_3d<U>{
            static_cast<U>(w), static_cast<U>(h), static_cast<U>(d)};
    }

    T w{0}, h{0}, d{0};

    inline T volume() const
    {
        return w * h * d;
    }

    inline f32 aspect() const
    {
        return f32(w) / f32(h);
    }

    inline T operator[](size_t i) const
    {
        switch(i)
        {
        case 0:
            return w;
        case 1:
            return h;
        case 2:
            return d;
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
        case 2:
            return d;
        default:
            return 0;
        }
    }
};

} // namespace geometry
} // namespace typing
