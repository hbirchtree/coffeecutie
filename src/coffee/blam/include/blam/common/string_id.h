#pragma once

#include "endian.h"

#include <bit>
#include <type_traits>

namespace blam {

/*! A string table entry; the low half is the index, zero names nothing */
template<std::endian E>
struct basic_string_id
{
    std::conditional_t<E == std::endian::little, libc_types::u32, endian_value<libc_types::u32, E>>
        raw;

    inline libc_types::u32 value() const
    {
        if constexpr(E == std::endian::little)
            return from_le(raw);
        else
            return raw.value();
    }

    inline libc_types::u16 index() const
    {
        return value() & 0xFFFF;
    }

    inline bool valid() const
    {
        return value() != 0;
    }
};

static_assert(sizeof(basic_string_id<std::endian::little>) == 4);
static_assert(sizeof(basic_string_id<std::endian::big>) == 4);

} // namespace blam
