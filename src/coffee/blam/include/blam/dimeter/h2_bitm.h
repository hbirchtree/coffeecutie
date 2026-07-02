#pragma once

#include "../volta/blam_bitm.h"

#include "h2_structures.h"

namespace blam::dimeter::bitm {

/* Halo 2 keeps Halo 1's numbering for these, including the extensions
 * declared in the volta header */
using blam::bitm::format_t;
using blam::bitm::type_t;

enum class flags_t : u16
{
    pow2       = 0x1,
    compressed = 0x2,
    palettized = 0x4,
    swizzled   = 0x8,
    linear     = 0x10,
    v16u16     = 0x20,
};

struct sprite_t;

/*!
 * \brief Analogous to Halo 1's sequence_t, layout from Assembly's bitm.xml.
 */
struct alignas(4) sequence_t
{
    bl_string name;
    i16       first_bitmap;
    i16       bitmap_count;
    u32       unknown[4];

    tag_block<sprite_t> sprites;
};

static_assert(offsetof(sequence_t, sprites) == 0x34);
static_assert(sizeof(sequence_t) == 0x3C);

struct alignas(4) sprite_t
{
    i16   bitmap_index;
    i16   unknown_0;
    u32   unknown_1;
    f32   left;
    f32   right;
    f32   top;
    f32   bottom;
    Vecf2 registration_point;
};

static_assert(sizeof(sprite_t) == 0x20);

/*!
 * \brief Halo 2's equivalent of Halo 1's image_t, grown from 48 to 116 bytes.
 * The pixel payload is split into up to three LOD mip chains, each behind a
 * raw_ref that may point into a shared cache file, plus a low-detail copy in
 * the map header's low-detail block.
 */
struct alignas(4) image_t
{
    bl_tag     sentinel; /*!< 'bitm' */
    i16        width;
    i16        height;
    i8         depth;
    u8         more_flags;
    type_t     type;
    format_t   format;
    flags_t    flags;
    bl_point_t reg_pnt;
    i16        mipmaps;
    i8         lod_adjust;
    u8         cache_usage;

    raw_ref pixels;         /*!< Unused on the maps inspected so far */
    raw_ref lod_offset[3];  /*!< LOD1 = highest detail */
    u32     unknown_0[3];
    u32     lod_size[3];
    u32     unknown_1[3];

    datum_index datum; /*!< Runtime-only */
    u32         unknown_2[4];

    u32      lowres_offset; /*!< Into the header's low-detail block */
    u32      lowres_size;
    format_t lowres_format;
    u16      unknown_3;
    u32      unknown_4[2];

    inline bool compressed() const
    {
        u16 f = from_le(static_cast<u16>(flags));
        return f & static_cast<u16>(flags_t::compressed);
    }

    inline bool swizzled() const
    {
        u16 f = from_le(static_cast<u16>(flags));
        return f & static_cast<u16>(flags_t::swizzled);
    }
};

static_assert(offsetof(image_t, pixels) == 0x18);
static_assert(offsetof(image_t, lod_size) == 0x34);
static_assert(offsetof(image_t, datum) == 0x4C);
static_assert(offsetof(image_t, lowres_offset) == 0x60);
static_assert(sizeof(image_t) == 0x74);

/*!
 * \brief bitm tag body; same overall shape as Halo 1's (settings + sequences
 * + images), different offsets and 8-byte tag_blocks instead of reflexives.
 */
struct alignas(4) header_t
{
    u16 type;
    u16 format;
    u16 usage;
    u16 flags;

    f32 detail_fade_factor;
    f32 sharpen_amount;
    f32 bump_height;

    u16 sprite_budget;
    i16 sprite_budget_count;

    i16 color_plate_width;
    i16 color_plate_height;

    u32 unknown_0[4];

    f32 blur_filter_size;
    f32 alpha_bias;
    i16 mipmap_count;

    u16 sprite_usage;
    i16 sprite_spacing;
    u16 force_format;

    tag_block<sequence_t> sequences;
    tag_block<image_t>    images;
};

static_assert(offsetof(header_t, blur_filter_size) == 0x2C);
static_assert(offsetof(header_t, sequences) == 0x3C);
static_assert(offsetof(header_t, images) == 0x44);
static_assert(sizeof(header_t) == 0x4C);

} // namespace blam::dimeter::bitm
