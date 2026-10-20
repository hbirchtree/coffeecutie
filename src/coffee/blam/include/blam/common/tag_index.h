#pragma once

#include "endian.h"
#include "tag_class.h"

#include <bit>
#include <type_traits>

namespace blam {

/*!
 * \brief A tag's handle: salt << 16 | position in the tag table. The salt
 * catches a handle from another map or a stale slot. The classes say which
 * tags the field may name; none admits any.
 */
template<std::endian E, tag_class_t... Classes>
struct basic_tag_index
{
    /* Little-endian handles keep their natural alignment */
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

    inline libc_types::u16 salt() const
    {
        return value() >> 16;
    }

    inline bool valid() const
    {
        return value() != 0xFFFFFFFF;
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wclass-conversion"

    /*! A typed handle is also an untyped one */
    template<typename Dummy = void>
    requires(sizeof...(Classes) > 0)
    inline operator basic_tag_index<E> const&() const
    {
        return *reinterpret_cast<basic_tag_index<E> const*>(this);
    }

#pragma GCC diagnostic pop
};

/*! Halo 1 and 2 store handles little-endian */
template<tag_class_t... Classes>
using tag_index = basic_tag_index<std::endian::little, Classes...>;

static_assert(sizeof(tag_index<>) == 4 && alignof(tag_index<>) == 4);
static_assert(sizeof(basic_tag_index<std::endian::big>) == 4);

} // namespace blam
