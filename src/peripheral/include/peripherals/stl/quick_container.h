#pragma once

#include <peripherals/stl/type_safety.h>
#include <stdexcept>
#include <utility>

namespace stl_types {

/*!
 * \brief Lightweight iterator pair posing as a range. Stores the begin/end
 * iterators by value — no std::function, no allocation. begin()/end() hand
 * out copies, so iteration never mutates the container itself.
 */
template<typename IteratorType>
struct quick_container
{
    quick_container(IteratorType&& begin_it, IteratorType&& end_it)
        : m_begin(std::move(begin_it))
        , m_end(std::move(end_it))
    {
    }

    IteratorType begin() const
    {
        return m_begin;
    }

    IteratorType end() const
    {
        return m_end;
    }

    decltype(auto) front() const
    {
        if(m_begin == m_end)
            Throw(std::out_of_range("no values in container"));
        return *m_begin;
    }

  private:
    IteratorType m_begin;
    IteratorType m_end;
};

} // namespace stl_types
