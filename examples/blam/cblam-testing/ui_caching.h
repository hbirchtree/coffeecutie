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

    void allocate_font_texture()
    {
        constexpr u32 kAtlasSize = 256;

        font_textures->alloc(
            gleam::size_3d<u32>{
                kAtlasSize, kAtlasSize, static_cast<u32>(m_cache.size())});
        font_sampler->alloc();
        api->debug().annotate(*font_textures, "fonts_r8");

        u32 layer = 0;
        for(auto& [id, font_item] : m_cache)
        {
            font_item.atlas_layer = layer;
            font_item.glyph_map.clear();

            auto chars_opt = font_item.font->characters.data(magic);
            if(!chars_opt.has_value())
            {
                layer++;
                continue;
            }

            /* Pack into CPU buffer with stride=kAtlasSize (always 4-byte
             * aligned) to avoid GL_UNPACK_ALIGNMENT issues with odd glyph
             * widths. */
            std::vector<u8> atlas_buf(kAtlasSize * kAtlasSize, 0);

            i32 cursor_x   = 0;
            i32 cursor_y   = 0;
            i32 row_height = 0;

            for(auto const& ch : chars_opt.value())
            {
                i32 bw = ch.bitmap_width;
                i32 bh = ch.bitmap_height;

                GlyphEntry entry;
                entry.advance       = ch.character_width;
                entry.bitmap_width  = static_cast<i16>(bw);
                entry.bitmap_height = static_cast<i16>(bh);
                entry.origin_x      = ch.origin.x;
                entry.origin_y      = ch.origin.y;

                if(bw > 0 && bh > 0)
                {
                    if(cursor_x + bw > static_cast<i32>(kAtlasSize))
                    {
                        cursor_x = 0;
                        cursor_y += row_height;
                        row_height = 0;
                    }
                    if(cursor_y + bh <= static_cast<i32>(kAtlasSize))
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
                                          (cursor_y + row) * kAtlasSize +
                                          cursor_x;
                                std::memcpy(
                                    dst, src + row * bw, static_cast<u32>(bw));
                            }
                        }
                        entry.atlas_x = static_cast<i16>(cursor_x);
                        entry.atlas_y = static_cast<i16>(cursor_y);
                        cursor_x += bw;
                        row_height = std::max(row_height, bh);
                    }
                }
                font_item.glyph_map[ch.character] = entry;
            }

            font_textures->upload(
                semantic::Bytes::ofContainer(atlas_buf).view,
                Veci3{0, 0, static_cast<i32>(layer)},
                Veci3{
                    static_cast<i32>(kAtlasSize),
                    static_cast<i32>(kAtlasSize),
                    1});
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
