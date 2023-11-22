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

struct FontItem
{
    blam::font const* font{nullptr};
    u32               atlas_layer;

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

    bool valid() const
    {
        return ui_element;
    }
};
