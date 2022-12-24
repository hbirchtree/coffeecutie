#pragma once

#include <functional>
#include <iterator>

namespace stl_types {

template<class Container>
inline bool all_of(
    Container const&                                                  c,
    std::function<bool(typename Container::value_type const&)> const& predicate)
{
    size_t sum = 0;
    for(auto const& v : c)
        sum += predicate(v) ? 1 : 0;
    return std::size(c) == sum;
}

} // namespace stl_types
