#pragma once

#include "endian.h"

#include <peripherals/semantic/chunk.h>

#include <bit>
#include <type_traits>

namespace blam {

/*! Index into a block of T as the tags store it, negative for none */
template<typename T, typename I = libc_types::i16, std::endian E = std::endian::little>
struct block_index
{
    using value_type = T;
    using raw_type   = I;

    /* Little-endian indices keep their natural alignment */
    std::conditional_t<E == std::endian::little, I, endian_value<I, E>> raw;

    inline I value() const
    {
        if constexpr(E == std::endian::little)
            return from_le(raw);
        else
            return raw.value();
    }

    inline bool valid() const
    {
        return value() >= 0;
    }

    inline explicit operator bool() const
    {
        return valid();
    }

    /*! The element named, null when unset or out of range */
    inline T const* resolve(semantic::Span<T const> items) const
    {
        I i = value();
        if(i < 0 || static_cast<size_t>(i) >= items.size())
            return nullptr;
        return &items[static_cast<size_t>(i)];
    }
};

static_assert(sizeof(block_index<int>) == 2 && alignof(block_index<int>) == 2);
static_assert(sizeof(block_index<int, libc_types::i16, std::endian::big>) == 2);

} // namespace blam
