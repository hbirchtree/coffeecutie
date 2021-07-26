#pragma once

#include <concepts>
#include <string>
#include <string_view>
#include <type_traits>

namespace semantic::concepts {

template<typename T>
concept is_string =
    (std::is_same_v<std::decay_t<T>, const char*> ||
     std::is_same_v<std::decay_t<T>, const wchar_t*>) &&
     requires (T v)
{
    { &v[0] };
};

}
