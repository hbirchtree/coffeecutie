#pragma once

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace stl_types {

namespace detail {

template<typename T>
struct as_const_range
{
    using type = T const;
};

template<typename T>
struct as_const_range<T&>
{
    using type = T const&;
};

template<typename T>
using as_const_range_t = typename as_const_range<T>::type;

} // namespace detail

// Much like Python's enumerate, provides an index for each element in an
// iterable container. It is a lazy view: nothing is copied out of the
// container and no storage is allocated.
// Container is `T&` when constructed from an lvalue, and `T` when constructed
// from a temporary, which is then held by value so the view cannot dangle.
template<typename Container>
struct enumerate
{
    using container_t = std::remove_reference_t<Container>;
    using iterator_t  = decltype(std::begin(std::declval<container_t&>()));
    using sentinel_t  = decltype(std::end(std::declval<container_t&>()));
    using reference_t = decltype(*std::declval<iterator_t&>());

    struct sentinel
    {
        sentinel_t it;
    };

    struct iterator
    {
        using value_type        = std::pair<std::size_t, reference_t>;
        using reference         = value_type;
        using pointer           = void;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        value_type operator*() const
        {
            return value_type(idx, *it);
        }

        iterator& operator++()
        {
            ++it;
            ++idx;
            return *this;
        }

        iterator operator++(int)
        {
            auto prev = *this;
            ++(*this);
            return prev;
        }

        bool operator==(iterator const& other) const
        {
            return it == other.it;
        }

        bool operator!=(iterator const& other) const
        {
            return it != other.it;
        }

        bool operator==(sentinel const& other) const
        {
            return it == other.it;
        }

        bool operator!=(sentinel const& other) const
        {
            return it != other.it;
        }

        iterator_t  it;
        std::size_t idx;
    };

    enumerate(container_t& c)
        requires(std::is_lvalue_reference_v<Container>)
        : m_container(c)
    {
    }

    enumerate(container_t const& c)
        requires(!std::is_lvalue_reference_v<Container>)
        : m_container(c)
    {
    }

    enumerate(container_t&& c)
        requires(!std::is_lvalue_reference_v<Container>)
        : m_container(std::move(c))
    {
    }

    iterator begin()
    {
        return iterator{std::begin(m_container), 0};
    }

    sentinel end()
    {
        return sentinel{std::end(m_container)};
    }

    std::size_t size()
    {
        return std::size(m_container);
    }

    bool empty()
    {
        return std::empty(m_container);
    }

  private:
    Container m_container;
};

template<typename C>
enumerate(C&) -> enumerate<C&>;
template<typename C>
enumerate(C&&) -> enumerate<C>;

template<typename Container>
struct const_enumerate : enumerate<detail::as_const_range_t<Container>>
{
    using base_t = enumerate<detail::as_const_range_t<Container>>;
    using base_t::base_t;
};

template<typename C>
const_enumerate(C&) -> const_enumerate<C&>;
template<typename C>
const_enumerate(C&&) -> const_enumerate<C>;

} // namespace stl_types
