#pragma once

#include <iterator>
#include <type_traits>

namespace stl_types {

template<class ContainerA, class ContainerB>
requires std::
    is_same_v<typename ContainerA::value_type, typename ContainerB::value_type>
inline bool equal(ContainerA const& c1, ContainerB const& c2)
{
    if(std::size(c1) != std::size(c2))
        return false;
    return std::equal(
        std::cbegin(c1), std::cend(c1), std::cbegin(c2), std::cend(c2));
}

} // namespace stl_types
