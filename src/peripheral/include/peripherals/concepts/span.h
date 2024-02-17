#pragma once

#include <concepts>
#include <cstddef>
#include <type_traits>

namespace semantic::concepts {

template<typename T>
concept Span = requires(T v) {
    std::is_same_v<decltype(v.data()), typename T::value_type*>;
    std::is_same_v<decltype(v.size()), typename T::size_type>;
    std::is_same_v<decltype(&v[0]), typename T::value_type*>;
};

template<typename T = std::byte>
struct null_span
{
    using value_type = T;
    using size_type  = size_t;

    template<typename T2>
    static inline null_span of(T2 size)
    {
        return {static_cast<size_t>(size)};
    }

    inline std::byte* data() const
    {
        return nullptr;
    }

    inline size_t size() const
    {
        return m_size;
    }

    inline std::byte const& operator[](size_t) const
    {
        return m_zero;
    }

    size_t    m_size{0};
    std::byte m_zero{0};
};

struct offset_span
{
    using value_type = std::byte;
    using size_type  = size_t;

    template<typename T2>
    requires std::is_integral_v<T2>
    static inline offset_span of(T2 offset)
    {
        return {static_cast<size_t>(offset)};
    }

    static inline offset_span of(void* offset)
    {
        static_assert(sizeof(size_t) == sizeof(void*));

        union ptr_to_int
        {
            void*  in;
            size_t out;
        } cast = {.in = offset};

        return {.m_offset = cast.out};
    }

    inline void* data() const
    {
        return reinterpret_cast<void*>(m_offset);
    }

    inline size_t size() const
    {
        return 1;
    }

    inline std::byte const& operator[](size_t) const
    {
        return m_zero;
    }

    size_t    m_offset{0};
    std::byte m_zero{0};
};

} // namespace semantic::concepts
