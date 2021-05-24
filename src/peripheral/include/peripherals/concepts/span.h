#pragma once

#include <concepts>
#include <type_traits>

namespace semantic::concepts {

template<template<typename E> typename T>
concept Span = requires(T<int> v)
{
    {std::is_same_v<decltype(v.data()), int*>};
    {std::is_same_v<decltype(v.size()), size_t>};
    {std::is_same_v<decltype(&v[0]), int*>};
};

}
