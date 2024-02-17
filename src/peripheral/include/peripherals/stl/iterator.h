#pragma once

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace stl_types {

template<typename Tag, typename T>
struct Iterator
{
    typedef std::remove_const_t<T> value_type;

    typedef Tag            iterator_category;
    typedef std::ptrdiff_t difference_type;
    typedef T*             pointer;
    typedef T&             reference;
};

using ForwardIteratorTag = std::forward_iterator_tag;

} // namespace stl_types
