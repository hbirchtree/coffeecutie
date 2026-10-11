#pragma once

#include "structures.h"

#include <array>
#include <cstring>

namespace blam::dimeter::stem {

/*!
 * shader_template (stem): which shader passes a template runs and how each
 * pass's bitmap/constant slots map onto the shad's postprocess blocks.
 * Layouts from Reclaimer's ShaderTemplateTag.cs (same on Xbox and Vista).
 */

/*! Packed range into a block: index in the low 9 bits, count above */
struct alignas(2) block_range_t
{
    u16 raw;

    inline u16 index() const
    {
        return from_le(raw) & 0x1FF;
    }

    inline u8 count() const
    {
        return static_cast<u8>(from_le(raw) >> 9);
    }
};

static_assert(sizeof(block_range_t) == 2);

/*! Pass entry, 10 bytes: tag_reference<> to the spas tag + implementation
 * range; too small for the 4-byte alignment of tag_reference<> */
struct alignas(2) pass_t
{
    std::array<u8, 10> raw;

    inline datum_index pass_id() const
    {
        datum_index id;
        std::memcpy(&id.raw, raw.data() + 4, 4);
        return id;
    }

    inline block_range_t implementations() const
    {
        block_range_t range;
        std::memcpy(&range.raw, raw.data() + 8, 2);
        return range;
    }
};

static_assert(sizeof(pass_t) == 10);

struct alignas(2) implementation_t
{
    block_range_t bitmaps;
    block_range_t pixel_constants;
    block_range_t vertex_constants;
};

static_assert(sizeof(implementation_t) == 6);

/*! Slot of the pass -> index into the shad's bitmaps / tiling block */
struct remapping_t
{
    u8 unknown_0[3];
    u8 block_index;
};

static_assert(sizeof(remapping_t) == 4);

struct alignas(4) properties_t
{
    u8                          unknown_0[16];
    tag_block<pass_t>           passes;
    tag_block<implementation_t> implementations;
    tag_block<remapping_t>      remappings;
};

static_assert(sizeof(properties_t) == 40);

struct alignas(4) header_t
{
    u8                      unknown_0[88];
    tag_block<properties_t> properties;
};

static_assert(offsetof(header_t, properties) == 88);

} // namespace blam::dimeter::stem
