#pragma once

#include <iterator>
#include <type_traits>

namespace stl_types {

template<
    typename ContainerType,
    typename Iterator = typename ContainerType::iterator>
struct slice
{
    using container_value_type = ContainerType;
    using difference_type      = typename container_value_type::difference_type;
    using iterator             = Iterator;
    using size_type            = typename container_value_type::size_type;

    iterator m_begin;
    iterator m_end;

    slice(iterator const& begin, iterator const& end)
        : m_begin(begin)
        , m_end(end)
    {
    }

    explicit slice(
        container_value_type& container,
        difference_type       offset = 0,
        difference_type       num    = 0)
        : m_begin(container.begin() + offset)
        , m_end(container.begin() + offset + num)
    {
    }

    iterator begin()
    {
        return m_begin;
    }

    iterator end()
    {
        return m_end;
    }

    typename container_value_type::size_type size() const
    {
        return m_end - m_begin;
    }
};

template<typename ContainerType>
requires(!std::is_const_v<ContainerType>)
static inline auto slice_num(
    ContainerType& c, typename ContainerType::difference_type num)
{
    return slice<ContainerType>(std::begin(c), std::begin(c) + num);
}

template<
    typename ContainerType,
    typename std::enable_if<std::is_const<ContainerType>::value>::type* =
        nullptr>
static inline auto slice_num(
    ContainerType& c, typename ContainerType::difference_type num)
{
    return slice<ContainerType>(std::cbegin(c), std::cbegin(c) + num);
}

} // namespace stl_types
