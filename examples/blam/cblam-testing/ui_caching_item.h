#pragma once

#include <blam/volta/blam_font.h>
#include <blam/volta/blam_ui.h>

#include "data_cache.h"

#include <string_view>

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

    /* A glyph the packer could not place: it still advances the cursor, but
     * there is nothing in the atlas to sample. */
    bool drawable() const
    {
        return bitmap_width > 0 && bitmap_height > 0;
    }
};

struct FontItem
{
    blam::font const*                   font{nullptr};
    u32                                 atlas_layer{0};
    u32                                 atlas_size{0};
    std::unordered_map<u16, GlyphEntry> glyph_map;

    bool valid() const
    {
        return font;
    }

    /* Advance width of a string in font units. Characters with no glyph
     * contribute nothing, which is what the draw loop does with them too. */
    f32 measure(std::u16string_view text) const
    {
        f32 width = 0.f;
        for(char16_t c : text)
            if(auto it = glyph_map.find(static_cast<u16>(c));
               it != glyph_map.end())
                width += it->second.advance;
        return width;
    }

    f32 line_height() const
    {
        return font ? static_cast<f32>(
                          font->ascend_height + font->descend_height)
                    : 0.f;
    }

    /* Walk a string, calling emit(glyph, x, y) for each drawable character
     * with its top-left corner already resolved against the pen. Layout lives
     * here so the UI renderer and anything else that wants text agree on it. */
    template<typename Emit>
    void for_each_glyph(
        std::u16string_view text, f32 start_x, f32 baseline_y, Emit&& emit) const
    {
        f32 pen = start_x;
        for(char16_t c : text)
        {
            auto it = glyph_map.find(static_cast<u16>(c));
            if(it == glyph_map.end())
                continue;
            GlyphEntry const& g = it->second;
            if(g.drawable())
                emit(g, pen + g.origin_x, baseline_y - g.origin_y);
            pen += g.advance;
        }
    }

    /* Sub-rect of the atlas to sample for a glyph, as (scale.xy, offset.xy).
     * This maps texel to pixel exactly when the quad is the glyph's own size;
     * the UI is laid out in 320x240 and scaled to the window, so in practice
     * the quad lands off-grid and the filter reaches just outside the rect.
     * What keeps that from showing is the gutter the packer leaves around
     * every glyph -- without it the stray tap lands on the neighbouring glyph,
     * and since this atlas is sampled as A8 (white plus coverage) it draws as
     * a white line down the side of every character. */
    Vecf4 glyph_uv(GlyphEntry const& g) const
    {
        f32 const size = static_cast<f32>(atlas_size ? atlas_size : 1);
        return Vecf4(
            static_cast<f32>(g.bitmap_width) / size,
            static_cast<f32>(g.bitmap_height) / size,
            static_cast<f32>(g.atlas_x) / size,
            static_cast<f32>(g.atlas_y) / size);
    }
};

struct UIElementItem
{
    blam::ui_element const*       ui_element{nullptr};
    std::vector<generation_idx_t> children;
    generation_idx_t              background;
    generation_idx_t              background_alt;

    /* For text_box widgets */
    generation_idx_t            font_id;
    std::vector<std::u16string> text_strings;

    bool visible{true};
    bool focused{false};
    bool selected{false};

    bool valid() const
    {
        return ui_element;
    }
};
