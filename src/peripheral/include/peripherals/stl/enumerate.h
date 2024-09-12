#pragma once

#include <vector>

namespace stl_types {

// Much like Python's enumerate, provides an index for each element in an
// iterable container
// In the future maybe make it operate on iterators, but I'm lazy
template<typename Container>
requires(!std::is_const_v<typename Container::value_type>)
struct enumerate
    : std::vector<std::pair<std::size_t, typename Container::reference>>
{
    enumerate(Container& c)
    {
        std::size_t i{0};
        for(auto& value : c)
            this->emplace_back(i++, std::ref(value));
    }
};

template<typename Container>
struct const_enumerate
    : std::vector<std::pair<std::size_t, typename Container::const_reference>>
{
    const_enumerate(Container& c)
    {
        std::size_t i{0};
        for(auto& value : c)
            this->emplace_back(i++, std::ref(value));
    }
};

} // namespace stl_types
