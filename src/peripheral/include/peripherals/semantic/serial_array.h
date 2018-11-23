#pragma once

#include <peripherals/identify/compiler/function_inlining.h>
#include <peripherals/libc/types.h>
#include <peripherals/semantic/chunk.h>
#include <peripherals/stl/types.h>

namespace semantic {

template<typename T>
struct SerialArray
{
    using size_type       = libc_types::szptr;
    using difference_type = libc_types::ptroff;

    SerialArray() : m_data(nullptr), m_size(0)
    {
    }

    SerialArray(Bytes const& source)
    {
        if((source.size % sizeof(T)) == 0)
        {
            m_data = C_RCAST<T const*>(source.data);
            m_size = source.size / sizeof(T);
        } else
            m_size = 0;
    }

    static const constexpr size_type npos = C_CAST<size_type>(-1);

    struct iterator : stl_types::Iterator<stl_types::ForwardIteratorTag, T>
    {
        iterator(SerialArray<T>* base) : m_base(base), m_idx(0)
        {
        }
        iterator() : m_base(nullptr), m_idx(npos)
        {
        }

        iterator& operator++()
        {
            m_idx++;
            if(m_idx >= m_base->m_size)
                m_idx = npos;
            return *this;
        }

        bool operator==(iterator const& other) const
        {
            return m_idx == other.m_idx;
        }
        bool operator!=(iterator const& other) const
        {
            return m_idx != other.m_idx;
        }

        T const& operator*() const
        {
#if MODE_DEBUG
            if(m_idx > m_base->m_size)
                Throw(std::out_of_range("invalid access"));
            else
#endif
                return (*m_base)[m_idx];
        }

      private:
        SerialArray<T>* m_base;
        size_type       m_idx;
    };

    NO_DISCARD iterator begin()
    {
        if(m_size == 0)
            return end();
        else
            return iterator(this);
    }

    NO_DISCARD iterator end()
    {
        return iterator();
    }

    T const& operator[](size_type i)
    {
#if MODE_DEBUG
        if(i > m_size)
            Throw(std::out_of_range("invalid access"));
        else
#endif
            return m_data[i];
    }

    size_type size() const
    {
        return m_size;
    }

  private:
    T const*  m_data;
    size_type m_size;
};

} // namespace semantic
