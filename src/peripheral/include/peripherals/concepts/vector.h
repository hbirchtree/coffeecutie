#pragma once

#include <concepts>
#include <peripherals/libc/types.h>
#include <type_traits>

namespace semantic::concepts {

namespace detail {

template<class T>
struct member_length
{
};

template<class T>
requires requires { typename std::integral_constant<size_t, T::length()>; }
struct member_length<T> : std::integral_constant<size_t, T::length()>
{
};

} // namespace detail

/* Component count of a vector type, or column count of a matrix; glm's
 * length() is not constexpr once it uses SIMD intrinsics. No value for types
 * without a fixed length. */
template<class T>
struct static_length : detail::member_length<T>
{
};

template<template<auto, typename, auto> class V, auto L, typename E, auto Q>
struct static_length<V<L, E, Q>> : std::integral_constant<size_t, L>
{
};

template<
    template<auto, auto, typename, auto> class M,
    auto C,
    auto R,
    typename E,
    auto Q>
struct static_length<M<C, R, E, Q>> : std::integral_constant<size_t, C>
{
};

template<class T, typename E, size_t N>
concept Vector = std::is_same_v<typename T::value_type, E> &&
                 static_length<T>::value == N && requires(T v) {
                     { v[0] = 1.f };
                     { v[N - 1] = 1.f };

                     { v += 1.f };
                     { v -= 1.f };
                     { v *= 1.f };
                     { v /= 1.f };
                     //    {v += T(1.f)};
                     //    {v -= T(1.f)};
                     //    {v *= T(1.f)};
                     //    {v /= T(1.f)};
                 };

template<class T, typename E, size_t M, size_t N>
concept Matrix = M == static_length<T>::value && N == static_length<T>::value &&
                 std::is_same_v<typename T::value_type, E> && requires(T v) {
                     std::is_same_v<decltype(v[0][0]), int>;
                     std::is_same_v<decltype(v * v), T>;
                 };

template<class T, typename E>
concept Size2D = std::is_same_v<typename T::value_type, E> && requires(T v) {
    //    { v.volume() };
    //    { v.aspect() };
    { v[0] };
};

template<class T, typename E>
concept Size3D = Size2D<T, E>;

} // namespace semantic::concepts
