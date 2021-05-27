#pragma once

#include <concepts>
#include <type_traits>

namespace semantic::concepts {

template<typename T>
concept Span = requires(T v)
{
    std::is_same_v<decltype(v.data()), typename T::value_type*>;
    std::is_same_v<decltype(v.size()), typename T::size_type>;
    std::is_same_v<decltype(&v[0]), typename T::value_type*>;
};

}
