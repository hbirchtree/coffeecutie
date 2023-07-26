#pragma once

#include <functional>

namespace stl_types {

template<typename IteratorType>
struct quick_container
{
    quick_container(
        std::function<IteratorType()>&& begin_con,
        std::function<IteratorType()>&& end_con) :
        m_begin(begin_con),
        m_end(end_con)
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

  private:
    std::function<IteratorType()> m_begin;
    std::function<IteratorType()> m_end;
};

}
