#pragma once

#include <concepts>
#include <peripherals/libc/types.h>

namespace semantic::concepts {

template<template<typename, size_t> class T>
concept Vector = requires(T<int, 2> v)
{
    {v.x() = 1.f};
    {v[0] = 1.f};
    {v[1] = 1.f};

    {v += 1.f};
    {v -= 1.f};
    {v *= 1.f};
    {v /= 1.f};
    {v += T(1.f)};
    {v -= T(1.f)};
    {v *= T(1.f)};
    {v /= T(1.f)};
};

template<template<typename, size_t, size_t> class T>
concept Matrix = requires(T<int, 2, 2> v)
{
    { std::is_same_v<decltype(v[0][0]), int> };
};

}
