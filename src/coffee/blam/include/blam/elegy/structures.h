#pragma once

#include "../common/endian.h"
#include "../common/magic.h"
#include "../common/reference.h"
#include "../common/tag_class.h"
#include "../common/tag_index.h"
#include "../common/tag_reference.h"
#include "../common/data_reference.h"
#include "../common/string_id.h"
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

/*! Halo 3 stores handles big-endian */
template<tag_class_t... Classes>
using tag_index = basic_tag_index<std::endian::big, Classes...>;
using datum_index = tag_index<>;

/*! A tag group fourcc; big-endian, so the bytes read in order */
using tag_class = basic_tag_class<std::endian::big>;

/*! Halo 3 string IDs: index, namespace and length */
struct string_id : basic_string_id<std::endian::big>
{
    inline u8 name_space() const
    {
        return (value() >> 16) & 0xFF;
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

template<tag_class_t... Classes>
using tag_reference = blam::tag_reference<xbox360_version_t, Classes...>;

static_assert(sizeof(tag_reference<>) == 0x10);

using data_reference = blam::data_reference<xbox360_version_t, heap_t::tag>;

static_assert(sizeof(data_reference) == 0x14);

struct bounds_f
{
    be<f32> min;
    be<f32> max;
};

static_assert(sizeof(bounds_f) == 8);

} // namespace blam::elegy
