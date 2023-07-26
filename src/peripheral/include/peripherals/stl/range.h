#pragma once

#include <peripherals/stl/type_safety.h>
#include "iterator.h"

namespace stl_types {

template<bool Reversed>
struct range_params
{
    static const constexpr bool reversed = Reversed;
};

using range_reversed = range_params<true>;

template<typename T = size_t, typename range_param = range_params<false>>
struct range
{
    struct iterator : Iterator<ForwardIteratorTag, T>
    {
        using difference_type = intptr_t;

        iterator(T end) : m_idx(end), m_end(end), m_stride(0)
        {
        }

        iterator(T start, T end, T stride) :
            m_idx(start), m_end(end), m_stride(stride)
        {
            bool correct     = (!range_param::reversed) && start > end;
            bool correct_rev = range_param::reversed && start > end;

            if(correct && correct_rev)
                Throw(std::out_of_range("invalid range"));
        }

        iterator& operator++()
        {
            m_idx += m_stride;

            if(m_idx >= m_end)
            {
                m_idx = m_end;
                m_stride = 0;
            }

            return *this;
        }

        iterator operator++(int)
        {
            iterator copy = *this;

            ++(*this);

            return copy;
        }

        bool operator==(iterator const& other) const
        {
            return m_idx == other.m_idx;
        }

        bool operator!=(iterator const& other) const
        {
            return m_idx != other.m_idx;
        }

        T operator*() const
        {
            if constexpr(range_param::reversed)
                return m_end - m_idx - 1;
            else
                return m_idx;
        }

        difference_type operator-(iterator const& other) const
        {
            return m_idx - other.m_idx;
        }

        auto operator+(intptr_t offset) const
        {
            auto out = *this;
            out.m_idx += offset;
            return out;
        }

      private:
        T m_idx;
        T m_end;
        T m_stride;
    };

    using value_type = T;

    range(T len, T stride = 1) : m_len(len), m_stride(stride)
    {
    }

    iterator begin()
    {
        return iterator(0, m_len, m_stride);
    }

    iterator end()
    {
        return iterator(m_len);
    }

  private:
    T m_len;
    T m_stride;
};

template<typename T = size_t>
using Range = range<T>;

template<typename T = size_t>
using range_rev = range<T, range_reversed>;

}
