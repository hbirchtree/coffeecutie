#pragma once

#include <concepts>
#include <peripherals/libc/types.h>

namespace semantic::concepts {

template<class T, typename E, size_t N>
concept Vector = std::is_same_v<typename T::value_type, E> && requires(T v)
{
    {v.x() = 1.f};
    {v[0] = 1.f};
    {v[1] = 1.f};

    {v += 1.f};
    {v -= 1.f};
    {v *= 1.f};
    {v /= 1.f};
//    {v += T(1.f)};
//    {v -= T(1.f)};
//    {v *= T(1.f)};
//    {v /= T(1.f)};
};

template<class T, typename E, size_t M, size_t N>
concept Matrix =
    M == T::col_size &&
    N == T::row_size &&
    std::is_same_v<typename T::value_type, E> &&
    requires(T v) {
    std::is_same_v<decltype(v[0][0]), int>;
    std::is_same_v<decltype(v * v), T>;
};

template<class T, typename E>
concept Size2D = std::is_same_v<typename T::value_type, E> && requires(T v)
{
    { v.volume() };
    { v.aspect() };
    { v[0] };
};

template<class T, typename E>
concept Size3D = Size2D<T, E>;

}
