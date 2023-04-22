#pragma once

#include "blam_base_types.h"
#include "blam_reflexive.h"
#include "blam_tag_ref.h"

namespace blam {

enum class font_flags_t : u16
{
    none,
};

struct alignas(32) font
{
    struct character_mapping
    {
        i16 index{}; /* Probably -1 for most?*/
    };
    struct alignas(4) character_t
    {
        u16        character;
        u16        character_width;
        u16        bitmap_width;
        u16        bitmap_height;
        bl_point_t origin;
        u16        scancode;
        u32        pixel_offset;
    };

    font_flags_t flags;
    u16          padding_;
    u16          ascend_height;
    u16          descend_height;
    u16          leadin_height;
    u16          leadin_width;

    u32 unknown_1[9];

    reflexive_t<character_mapping> character_table;

    tagref_typed_t<tag_class_t::font> bold;
    tagref_typed_t<tag_class_t::font> italic;
    tagref_typed_t<tag_class_t::font> condense;
    tagref_typed_t<tag_class_t::font> underline;

    reflexive_t<character_t> characters;
    reflexive_t<char>        pixels;
};

static_assert(sizeof(font) == 160);
static_assert(offsetof(font, character_table) == 48);
static_assert(offsetof(font, pixels) == 136);
static_assert(offsetof(font::character_t, pixel_offset) == 16);
static_assert(sizeof(font::character_t) == 20);

} // namespace blam
