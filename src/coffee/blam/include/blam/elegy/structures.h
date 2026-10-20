#pragma once

#include "../common/endian.h"
#include "../common/tag_class.h"
#include "../volta/blam_base_types.h"
#include "../volta/blam_structures.h"

#include "versions.h"

#include <peripherals/enum/helpers.h>

#include <array>
#include <cstring>
#include <string_view>

namespace blam::elegy {

using typing::vector_types::Matf4;

static_assert(sizeof(be<u32>) == 4 && alignof(be<u32>) == 1);
static_assert(sizeof(be<Vecf3>) == 12);

enum class cache_type_t : i16
{
    campaign   = 0,
    multiplayer = 1,
    mainmenu   = 2,
    shared     = 3,
    campaign_shared = 4,
};

/*!
 * \brief Handle-style ID used for tags and resources; low 16 bits index the
 * table, high 16 bits are a salt value.
 */
struct datum_index
{
    be<u32> raw;

    inline u16 index() const
    {
        return raw.value() & 0xFFFF;
    }

    inline u16 salt() const
    {
        return raw.value() >> 16;
    }

    inline bool valid() const
    {
        return raw.value() != 0xFFFFFFFF;
    }
};

static_assert(sizeof(datum_index) == 4);

/*! A tag group fourcc; big-endian, so the bytes read in order */
using tag_class = basic_tag_class<std::endian::big>;

/*!
 * \brief Halo 3 string IDs pack an index, a namespace and the string's
 * length (16:8:8); namespaces map to offsets in the string table.
 */
struct string_id
{
    be<u32> raw;

    inline u16 index() const
    {
        return raw.value() & 0xFFFF;
    }

    inline u8 name_space() const
    {
        return (raw.value() >> 16) & 0xFF;
    }

    inline bool valid() const
    {
        return raw.value() != 0;
    }
};

static_assert(sizeof(string_id) == 4);

/*!
 * \brief Address-space sections named in the file header. Each has a
 * virtual base and an offset mask, the mask taking base to a file offset.
 */
enum class section_t
{
    debug,
    resource,
    tag,
    locale,
};

/*!
 * \brief Tag data pointer translation. Tag pointers are 360 virtual
 * addresses; they translate as tag_offset + (pointer - virtual_base).
 */
struct tag_magic
{
    byte_t const* base{nullptr};
    u32           max_size{0};
    u32           tag_offset{0};
    u32           virtual_base{0};

    inline optional<u32> to_offset(u32 pointer, u32 size = 0) const
    {
        u32 offset = tag_offset + (pointer - virtual_base);
        if(offset >= max_size || size > max_size - offset)
            return std::nullopt;
        return offset;
    }

    inline byte_t const* resolve(u32 pointer, u32 size = 0) const
    {
        if(auto offset = to_offset(pointer, size); base && offset)
            return base + *offset;
        return nullptr;
    }

    template<typename T>
    inline T const* as(u32 pointer, u32 count = 1) const
    {
        return reinterpret_cast<T const*>(resolve(pointer, count * sizeof(T)));
    }
};

/*!
 * \brief A tag block: count, virtual address and a runtime-only word.
 * Blocks inside resource definitions have the same shape but are resolved
 * through the resource's fixups instead (see resource_view).
 */
template<typename T>
struct tag_block
{
    using value_type = T;

    be<i32> count_raw;
    be<u32> pointer_raw;
    u32     runtime;

    inline i32 count() const
    {
        return count_raw;
    }

    inline u32 pointer() const
    {
        return pointer_raw;
    }

    inline result<Span<T const>, error_msg> data(tag_magic const& magic) const
    {
        i32 num = count();
        if(num == 0)
            return Span<T const>();
        if(num < 0)
            return stl_types::failure("negative tag_block count"sv);
        auto const* items = magic.as<T>(pointer(), static_cast<u32>(num));
        if(!items)
            return stl_types::failure("tag_block pointer out of bounds"sv);
        return Span<T const>(items, static_cast<size_t>(num));
    }

    /*! The elements, none when the block is empty or out of bounds */
    inline Span<T const> span(tag_magic const& magic) const
    {
        return data(magic).value_or(Span<T const>());
    }
};

static_assert(sizeof(tag_block<u32>) == 0xC);

/*! Index into a tag block of T as the tags store it, -1 for none */
template<typename T, typename I = i16>
struct block_index
{
    using value_type = T;

    be<I> raw;

    inline I value() const
    {
        return raw;
    }

    inline bool valid() const
    {
        return value() >= 0;
    }

    inline explicit operator bool() const
    {
        return valid();
    }

    inline T const* resolve(Span<T const> block) const
    {
        if(!valid() || static_cast<size_t>(value()) >= block.size())
            return nullptr;
        return &block[static_cast<size_t>(value())];
    }
};

static_assert(sizeof(block_index<u8>) == 2);

/*! A reference to another tag; the group is repeated in front of the id */
struct tag_reference
{
    tag_class           group;
    std::array<u32, 2>  runtime;
    datum_index         id;

    inline bool valid() const
    {
        return id.valid();
    }
};

static_assert(sizeof(tag_reference) == 0x10);

/*! An untyped blob of tag data */
struct data_reference
{
    be<i32>            size;
    std::array<u32, 2> runtime_0;
    be<u32>            pointer;
    u32                runtime_1;

    inline Span<byte_t const> data(tag_magic const& magic) const
    {
        i32 bytes = size;
        if(bytes <= 0)
            return {};
        auto const* out = magic.resolve(pointer, static_cast<u32>(bytes));
        if(!out)
            return {};
        return Span<byte_t const>(out, static_cast<size_t>(bytes));
    }
};

static_assert(sizeof(data_reference) == 0x14);

struct bounds_f
{
    be<f32> min;
    be<f32> max;
};

static_assert(sizeof(bounds_f) == 8);

} // namespace blam::elegy
