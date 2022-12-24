#pragma once

#include <functional>

namespace stl_types {

template<class Container>
inline void for_each(
    Container const&                                                  c,
    std::function<void(typename Container::value_type const&)> const& predicate)
{
    for(auto const& v : c)
        predicate(v);
}

} // namespace stl_types
