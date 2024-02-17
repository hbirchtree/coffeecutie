#pragma once

#include <concepts>
#include <iterator>

namespace semantic::concepts {

template<typename T>
concept is_container = requires(T v) {
    {
        std::begin(v)
    };
    {
        std::end(v)
    };
    {
        std::cbegin(v)
    };
    {
        std::cend(v)
    };
};

}
