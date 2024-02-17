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
        i16        character_width;
        i16        bitmap_width;
        i16        bitmap_height;
        bl_point_t origin;
        u16        scancode;
        u32        pixel_offset;

        static constexpr u16 no_scancode =
            static_cast<u16>(static_cast<i16>(-1));
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
    u32                      pixels_offset;

    reflexive_t<char> pixel_data(u32 offset = 0, u32 size = 0) const
    {
        auto p   = pixels;
        p.offset = pixels_offset + offset;
        p.count -= offset;
        if(size != 0 && size < p.count)
            p.count = size;
        return p;
    }

    using character_pair_t =
        std::optional<std::tuple<character_t const*, Span<const char>>>;

    character_pair_t character(u16 c, magic_data_t const& magic) const
    {
        character_t const* selection{nullptr};
        auto               char_opt = characters.data(magic);
        if(!char_opt.has_value())
            return std::nullopt;
        auto all_chars = char_opt.value();
        for(auto const& chara : all_chars)
        {
            if(chara.character == c)
            {
                selection = &chara;
                break;
            }
        }
        if(!selection)
            return std::nullopt;
        auto data_size = std::max<u16>(selection->bitmap_width, 0u) *
                         std::max<u16>(selection->bitmap_height, 0u);
        auto data = pixel_data(selection->pixel_offset, data_size).data(magic);
        if(!data.has_value())
            return std::nullopt;
        return std::make_tuple(selection, data.value());
    }
};

static_assert(sizeof(font) == 160);
static_assert(offsetof(font, character_table) == 48);
static_assert(offsetof(font, pixels) == 136);
static_assert(offsetof(font::character_t, pixel_offset) == 16);
static_assert(sizeof(font::character_t) == 20);

} // namespace blam
