#pragma once

#include <blam/volta/blam_font.h>
#include <blam/volta/blam_ui.h>

#include "data_cache.h"

using libc_types::f32;
using libc_types::i16;
using libc_types::i32;
using libc_types::u16;
using libc_types::u32;
using semantic::Span;
using stl_types::range;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf3;
using typing::vector_types::Vecf4;
using typing::vector_types::Veci2;
using typing::vector_types::Veci3;
using typing::vector_types::Veci4;

struct GlyphEntry
{
    i16 atlas_x{0}, atlas_y{0};
    i16 bitmap_width{0}, bitmap_height{0};
    i16 origin_x{0}, origin_y{0};
    i16 advance{0};
};

struct FontItem
{
    blam::font const* font{nullptr};
    u32               atlas_layer{0};
    std::unordered_map<u16, GlyphEntry> glyph_map;

    bool valid() const
    {
        return font;
    }
};

struct UIElementItem
{
    blam::ui_element const*       ui_element{nullptr};
    std::vector<generation_idx_t> children;
    generation_idx_t              background;
    generation_idx_t              background_alt;

    /* For text_box widgets */
    generation_idx_t          font_id;
    std::vector<std::u16string> text_strings;

    bool visible{true};
    bool focused{false};
    bool selected{false};

    bool valid() const
    {
        return ui_element;
    }
};
