#pragma once

#include <functional>
#include <peripherals/stl/type_safety.h>
#include <stdexcept>

namespace stl_types {

template<typename IteratorType>
struct quick_container
{
    quick_container(
        std::function<IteratorType()>&& begin_con,
        std::function<IteratorType()>&& end_con)
        : m_begin(begin_con)
        , m_end(end_con)
    {
    }

    IteratorType begin()
    {
        return m_begin();
    }

    IteratorType end()
    {
        return m_end();
    }

    typename IteratorType::value_type& front()
    {
        if(begin() == end())
            Throw(std::out_of_range("no values in container"));
        return (*begin());
    }

  private:
    std::function<IteratorType()> m_begin;
    std::function<IteratorType()> m_end;
};

} // namespace stl_types
