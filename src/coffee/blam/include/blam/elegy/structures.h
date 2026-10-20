#pragma once

#include "../common/endian.h"
#include "../common/magic.h"
#include "../common/reference.h"
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
 * addresses; they translate as file_offset + (pointer - virtual_base).
 */
using tag_magic = heap_range<heap_t::tag>;

/*!
 * \brief A tag block: count, virtual address and a runtime-only word.
 * Blocks inside resource definitions have the same shape but are resolved
 * through the resource's fixups instead (see resource_view).
 */
template<typename T>
using tag_block = reference<T, xbox360_version_t, atlas_type_t::map_file, heap_t::tag>;

static_assert(sizeof(tag_block<u32>) == 0xC);

/*! Index into a tag block of T as the tags store it, -1 for none */
template<typename T, typename I = i16>
using block_index = blam::block_index<T, I, std::endian::big>;

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

    inline Span<u8 const> data(tag_magic const& magic) const
    {
        i32 bytes = size;
        if(bytes <= 0)
            return {};
        auto const* out = magic.resolve(pointer, static_cast<u32>(bytes));
        if(!out)
            return {};
        return Span<u8 const>(out, static_cast<size_t>(bytes));
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
