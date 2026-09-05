#pragma once

#include "bitmap_cache.h"
#include "caching_item.h"
#include "shader_cache.h"
#include "ui_caching_item.h"

#include <blam/volta/blam_scenario.h>
#include <peripherals/stl/magic_enum.hpp>

#include <cstring>

using libc_types::u8;

template<typename V>
struct FontCache
    : DataCache<FontItem, blam::font const*, blam::tagref_t const&>
    , compo::SubsystemBase
{
    using type = FontCache<V>;

    FontCache(gfx::api* allocator)
        : font_textures(
              std::make_shared<gfx::compat::texture_2da_t>(
                  allocator, PixDesc(pix_fmt::R8), 1))
        , font_sampler(font_textures->sampler())
        , api(allocator)
    {
    }

    blam::tag_index_view<V>                     index;
    blam::map_ptr                               magic;
    std::shared_ptr<gfx::compat::texture_2da_t> font_textures;
    std::shared_ptr<gfx::sampler_t>             font_sampler;
    gfx::api*                                   api;
    u32                                         atlas_layer{0};

    inline void load_from(blam::map_container<V> const& map)
    {
        index       = blam::tag_index_view(map);
        magic       = map.magic;
        atlas_layer = 0;
        evict_all();
    }

    virtual FontItem predict_impl(blam::tagref_t const& font_tag)
    {
        blam::font const* font = get_id(font_tag);
        return FontItem{
            .font        = font,
            .atlas_layer = 0,
        };
    }

    virtual blam::font const* get_id(blam::tagref_t const& font_tag)
    {
        if(auto tag_it = index.find(font_tag); tag_it == index.end())
            return nullptr;
        else if(auto data = (*tag_it).template data<blam::font>(magic);
                !data.has_value())
            return nullptr;
        else
            return data.value();
    }

    /* One transparent texel between glyphs, so a filter tap at the edge of one
     * cannot reach into the next. FontItem::glyph_uv() handles the other half
     * of the problem, the tap that leaves the glyph's own rect. */
    static constexpr i32 kGlyphGutter = 1;

    /* Shelf packer shared by the sizing pass and the upload pass. Calls
     * place(character, x, y) for every glyph, with x/y negative when it has no
     * home; returns false if any glyph did not fit. */
    template<typename Place>
    static bool pack_glyphs(
        Span<const blam::font::character_t> chars, i32 size, Place&& place)
    {
        i32  cursor_x = 0, cursor_y = 0, row_height = 0;
        bool fits = true;

        for(auto const& ch : chars)
        {
            i32 const bw = ch.bitmap_width, bh = ch.bitmap_height;
            if(bw <= 0 || bh <= 0)
            {
                place(ch, -1, -1);
                continue;
            }
            i32 const step_w = bw + kGlyphGutter;
            i32 const step_h = bh + kGlyphGutter;
            if(cursor_x + step_w > size)
            {
                cursor_x = 0;
                cursor_y += row_height;
                row_height = 0;
            }
            if(cursor_y + step_h > size)
            {
                /* Out of shelves. Report it rather than placing the glyph at
                 * 0,0, which is what the previous packer did -- the entry kept
                 * its default atlas coordinates and drew whichever glyph
                 * happened to sit at the atlas origin. */
                fits = false;
                place(ch, -1, -1);
                continue;
            }
            place(ch, cursor_x, cursor_y);
            cursor_x += step_w;
            row_height = std::max(row_height, step_h);
        }
        return fits;
    }

    Span<const blam::font::character_t> characters_of(FontItem const& item)
    {
        if(!item.font)
            return {};
        auto chars = item.font->characters.data(magic);
        return chars.has_value() ? chars.value()
                                 : Span<const blam::font::character_t>{};
    }

    void allocate_font_texture()
    {
        /* Every layer of the array is the same size, so the atlas has to be
         * big enough for the largest font; grow until the shelf packer stops
         * running out of room. */
        constexpr i32 kMinAtlas = 256;
        constexpr i32 kMaxAtlas = 2048;

        i32 atlas_size = kMinAtlas;
        while(atlas_size < kMaxAtlas)
        {
            bool all_fit = true;
            for(auto& [id, font_item] : m_cache)
            {
                auto chars = characters_of(font_item);
                if(chars.empty())
                    continue;
                if(!pack_glyphs(chars, atlas_size, [](auto const&, i32, i32) {}))
                {
                    all_fit = false;
                    break;
                }
            }
            if(all_fit)
                break;
            atlas_size *= 2;
        }

        cDebug(
            "Font atlas: {}x{} x{} layers", atlas_size, atlas_size,
            m_cache.size());

        font_textures->alloc(
            gleam::size_3d<u32>{
                static_cast<u32>(atlas_size),
                static_cast<u32>(atlas_size),
                static_cast<u32>(m_cache.size())});
        font_sampler->alloc();
        api->debug().annotate(*font_textures, "fonts_r8");

        u32 layer = 0;
        for(auto& [id, font_item] : m_cache)
        {
            font_item.atlas_layer = layer;
            font_item.atlas_size  = static_cast<u32>(atlas_size);
            font_item.glyph_map.clear();

            auto chars = characters_of(font_item);
            if(chars.empty())
            {
                layer++;
                continue;
            }

            /* Pack into a CPU buffer with stride=atlas_size (a power of two,
             * so always 4-byte aligned) to avoid GL_UNPACK_ALIGNMENT issues
             * with odd glyph widths. */
            std::vector<u8> atlas_buf(
                static_cast<size_t>(atlas_size) * atlas_size, 0);

            bool const fits = pack_glyphs(
                chars,
                atlas_size,
                [&](blam::font::character_t const& ch, i32 x, i32 y) {
                    GlyphEntry entry;
                    entry.advance  = ch.character_width;
                    entry.origin_x = ch.origin.x;
                    entry.origin_y = ch.origin.y;

                    i32 const bw = ch.bitmap_width, bh = ch.bitmap_height;
                    if(x >= 0 && y >= 0)
                    {
                        auto data_size =
                            static_cast<u32>(bw) * static_cast<u32>(bh);
                        auto pix = font_item.font
                                       ->pixel_data(ch.pixel_offset, data_size)
                                       .data(magic);
                        if(pix.has_value())
                        {
                            auto const* src =
                                reinterpret_cast<u8 const*>(pix.value().data());
                            for(i32 row = 0; row < bh; row++)
                            {
                                u8* dst = atlas_buf.data() +
                                          static_cast<size_t>(y + row) *
                                              atlas_size +
                                          x;
                                std::memcpy(
                                    dst, src + row * bw, static_cast<u32>(bw));
                            }
                        }
                        entry.atlas_x       = static_cast<i16>(x);
                        entry.atlas_y       = static_cast<i16>(y);
                        entry.bitmap_width  = static_cast<i16>(bw);
                        entry.bitmap_height = static_cast<i16>(bh);
                    }
                    /* An unplaced glyph keeps its advance and stays
                     * undrawable, so text still spaces correctly. */
                    font_item.glyph_map[ch.character] = entry;
                });

            if(!fits)
                cWarning(
                    "Font atlas overflowed at {}x{}; some glyphs will not "
                    "render",
                    atlas_size,
                    atlas_size);

            font_textures->upload(
                semantic::Bytes::ofContainer(atlas_buf).view,
                Veci3{0, 0, static_cast<i32>(layer)},
                Veci3{atlas_size, atlas_size, 1});
            layer++;
        }
    }
};

template<typename V>
struct UIElementCache
    : DataCache<UIElementItem, blam::ui_element const*, blam::tagref_t const&>
    , compo::SubsystemBase
{
    using type = UIElementCache<V>;

    UIElementCache(BitmapCache<V>& bitm_cache, FontCache<V>& font_cache)
        : bitm_cache(bitm_cache)
        , font_cache(font_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index = blam::tag_index_view(map);
        magic = map.magic;
        evict_all();
    }

    virtual UIElementItem predict_impl(blam::tagref_t const& ui_tag)
    {
        using widget_type = blam::ui_element::widget_type_t;

        auto ui_el = get_id(ui_tag);
        // cDebug(
        //     "UI element of type: {}",
        //     magic_enum::enum_name(ui_el->widget_type));
        UIElementItem out{
            .ui_element = ui_el,
            .children   = {},
            .background = {},
        };
        if(ui_el->background.valid())
        {
            auto all_bitms = bitm_cache.resolve_all(ui_el->background);
            out.background = all_bitms.front();
            if(all_bitms.size() > 1)
                out.background_alt = all_bitms.at(1);
        }
        switch(ui_el->widget_type)
        {
        case widget_type::column_list:
        case widget_type::container: {
            using child_widget_t = blam::ui_element::child_widget_t;
            auto children        = ui_el->child_widgets.data(magic).value();
            for(child_widget_t const& child : children)
            {
                if(auto c = predict(child.widget); c.valid())
                    out.children.push_back(c);
            }
            break;
        }
        case widget_type::text_box: {
            auto const& tb = ui_el->text_box;
            if(tb.font.valid())
                out.font_id = font_cache.predict(tb.font);
            if(tb.unicode_strings.valid())
            {
                if(auto us_data = index.template data<blam::ui::unicode_string>(
                       tb.unicode_strings);
                   us_data.has_value())
                {
                    auto const* us = us_data.value();
                    if(auto subs = us->sub_strings.data(magic);
                       subs.has_value())
                    {
                        for(auto const& ref : subs.value())
                        {
                            if(auto s = ref.str(magic); !s.has_error())
                                out.text_strings.emplace_back(s.value());
                            else
                                out.text_strings.emplace_back();
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        return out;
    }

    virtual blam::ui_element const* get_id(blam::tagref_t const& ui_tag)
    {
        if(auto data = index.template data<blam::ui_element>(ui_tag);
           !data.has_value())
            return nullptr;
        else
            return *data;
    }

    std::vector<generation_idx_t> explore(
        blam::tagref_t const& widget_collection)
    {
        auto collection_ =
            index.template data<blam::ui_item_collection>(widget_collection);
        if(!collection_.has_value())
            return {};
        blam::ui_item_collection const* collection = collection_.value();
        auto widgets = *index.deref(collection->widget_definitions);
        std::vector<generation_idx_t> root_widgets;
        std::transform(
            std::begin(widgets),
            std::end(widgets),
            std::back_inserter(root_widgets),
            [this](auto const& widget) { return predict(widget.definition); });
        return root_widgets;
    }

    blam::tag_index_view<V> index;
    blam::map_ptr           magic;
    BitmapCache<V>&         bitm_cache;
    FontCache<V>&           font_cache;
};
