#pragma once

#include <map>
#include <vector>

namespace stl_types {

template<typename V, typename K>
std::vector<V> values(std::map<K, V> const& source)
{
    std::vector<V> out;
    for(auto const& [key, value] : source)
        out.push_back(value);
    return out;
}

}
