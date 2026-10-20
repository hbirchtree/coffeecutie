#pragma once

#include "endian.h"
#include "tag_class.h"

#include <peripherals/semantic/chunk.h>

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

    template<tag_class_t... Other>
    inline bool operator==(basic_tag_index<E, Other...> const& other) const
    {
        return value() == other.value();
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

template<std::endian E, tag_class_t... Classes>
inline libc_types::u32 format_as(basic_tag_index<E, Classes...> const& id)
{
    return id.value();
}

/*!
 * \brief The tag table entry a handle names: the low half is the position,
 * and the entry's salt must match the high half. Null for a null handle, a
 * position past the table or a stale salt.
 */
template<typename Entry, typename SaltOf>
inline Entry const* lookup_tag(
    semantic::Span<Entry const> table, libc_types::u32 handle, SaltOf&& salt_of)
{
    if(handle == 0xFFFFFFFF)
        return nullptr;
    libc_types::u32 const position = handle & 0xFFFF;
    if(position >= table.size())
        return nullptr;
    Entry const& entry = table[position];
    if(static_cast<libc_types::u16>(salt_of(entry)) != (handle >> 16))
        return nullptr;
    return &entry;
}

/*! Halo 1 and 2 store handles little-endian */
template<tag_class_t... Classes>
using tag_index = basic_tag_index<std::endian::little, Classes...>;

static_assert(sizeof(tag_index<>) == 4 && alignof(tag_index<>) == 4);
static_assert(sizeof(basic_tag_index<std::endian::big>) == 4);

} // namespace blam
