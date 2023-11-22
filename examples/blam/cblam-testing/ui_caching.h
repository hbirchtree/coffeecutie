#pragma once

#include "bitmap_cache.h"
#include "caching_item.h"
#include "shader_cache.h"
#include "ui_caching_item.h"

using libc_types::u8;

template<typename V>
struct FontCache
    : DataCache<FontItem, blam::font const*, blam::tagref_t const&>,
      compo::SubsystemBase
{
    using type = FontCache<V>;

    FontCache(gfx::api* allocator) :
        font_textures(std::make_shared<gfx::compat::texture_2da_t>(
            allocator, PixDesc(PixFmt::R8), 1)),
        font_sampler(font_textures->sampler()), api(allocator)
    {
    }

    blam::tag_index_view<V>                     index;
    blam::magic_data_t                          magic;
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
        auto              name = font_tag.to_name().to_string(magic);
        auto              data = font->pixel_data().data(magic);
        return FontItem{
            .font        = font,
            .atlas_layer = 0,
        };
    }

    virtual blam::font const* get_id(blam::tagref_t const& font_tag)
    {
        if(auto tag_it = index.find(font_tag); tag_it == index.end())
            return nullptr;
        else
            return (*tag_it).template data<blam::font>(magic).value();
    }

    void allocate_font_texture()
    {
        using namespace std::string_view_literals;

        Vecui3 total_size{0, 0, m_cache.size()};

        font_textures->alloc(
            gleam::size_3d<u32>{256, 256, static_cast<u32>(m_cache.size())});
        font_sampler->alloc();
        api->debug().annotate(*font_textures, "fonts_r8");

        u32 i = 0;
        for(auto& [id, font_] : m_cache)
        {
            FontItem& font   = font_;
            font.atlas_layer = i;

            u32 advance = 0;
            for(auto c : "HALO IS COOL"sv)
            {
                auto c_ = font.font->character(c, magic);
                if(!c_.has_value())
                    continue;
                auto [desc, data] = c_.value();
                if(desc->bitmap_width < 1 || desc->bitmap_height < 1)
                {
                    advance += desc->character_width;
                    continue;
                }
                font_textures->upload(
                    data,
                    Veci3{advance, 0, i},
                    Veci3{desc->bitmap_width, desc->bitmap_height, 1});
                advance += desc->character_width;
            }
            i++;
        }
    }
};

template<typename V>
struct UIElementCache
    : DataCache<UIElementItem, blam::ui_element const*, blam::tagref_t const&>,
      compo::SubsystemBase
{
    using type = UIElementCache<V>;

    UIElementCache(BitmapCache<V>& bitm_cache, FontCache<V>& font_cache) :
        bitm_cache(bitm_cache), font_cache(font_cache)
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
        cDebug(
            "UI element of type: {}",
            magic_enum::enum_name(ui_el->widget_type));
        UIElementItem out{
            .ui_element = ui_el,
        };
        if(ui_el->background.valid())
            out.background
                = bitm_cache.predict(ui_el->background.to_plain(), 0);
        switch(ui_el->widget_type)
        {
        case widget_type::column_list:
        case widget_type::container: {
            using child_widget_t = blam::ui_element::child_widget_t;
            auto children        = ui_el->child_widgets.data(magic).value();
            for(child_widget_t const& child : children)
            {
                if(auto c = predict(child.widget.to_plain()); c.valid())
                    out.children.push_back(c);
            }
            break;
        }
        }
        return out;
    }

    virtual blam::ui_element const* get_id(blam::tagref_t const& ui_tag)
    {
        if(auto tag_it = index.find(ui_tag); tag_it == index.end())
            return nullptr;
        else
            return (*tag_it).template data<blam::ui_element>(magic).value();
    }

    std::vector<generation_idx_t> explore(
        blam::tagref_t const& widget_collection)
    {
        auto collection_
            = index.template data<blam::ui_item_collection>(widget_collection);
        if(!collection_.has_value())
            return {};
        blam::ui_item_collection const* collection = collection_.value();
        auto widgets = collection->widget_definitions.data(magic).value();
        std::vector<generation_idx_t> root_widgets;
        std::transform(
            std::begin(widgets),
            std::end(widgets),
            std::back_inserter(root_widgets),
            [this](auto const& widget) {
                return predict(widget.definition.to_plain());
            });
        return root_widgets;
    }

    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;
    BitmapCache<V>&         bitm_cache;
    FontCache<V>&           font_cache;
};
