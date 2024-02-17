#pragma once

#include "caching.h"
#include "components.h"
#include "data.h"
#include <coffee/imgui/imgui_binding.h>

template<typename V>
using BlamTextureBrowserManifest = compo::SubsystemManifest<
    empty_list_t,
    type_list_t<BitmapCache<V>, gfx::system>,
    empty_list_t>;

template<typename V>
struct BlamTextureBrowser
    : compo::RestrictedSubsystem<
          BlamTextureBrowser<V>,
          BlamTextureBrowserManifest<V>>
{
    using type  = BlamTextureBrowser<V>;
    using Proxy = compo::proxy_of<BlamTextureBrowserManifest<V>>;

    BlamTextureBrowser()
    {
        compo::SubsystemBase::priority = 2048;
    }

    void start_restricted(Proxy& e, time_point const&)
    {
        if(!m_map)
            return;

        auto& api = e.template subsystem<gfx::system>();

        if(ImGui::Begin("Textures"))
        {
            list_textures(e);

            if(m_selected.valid())
            {
                auto const& features = api.feature_info();
                if(m_updated)
                {
                    if(features.texture.image_copy)
                        show_texture_view(e);
                    else
                        show_texture_copy(e);
                    m_updated = false;
                }
                show_texture_imgui();
            }
        }
        ImGui::End();
    }

    void end_restricted(Proxy&, time_point const&)
    {
    }

    void show_texture_view(Proxy& e)
    {
        BitmapCache<V>& bitmap_cache = e.template subsystem<BitmapCache<V>>();
        auto&           buckets      = bitmap_cache.tex_buckets;
        auto            buckit       = buckets.find(m_selected.image.bucket);

        if(buckit == buckets.end() || !buckit->second.surface)
        {
            ImGui::Text("[Texture not allocated]");
            return;
        }
        gleam::texture_t& bucket = *buckit->second.surface;

        auto view_params = gleam::textures::view_params{
            .layer =
                {
                    .min_  = static_cast<i32>(m_selected.image.layer),
                    .count = 1,
                },
            .mip =
                {
                    .min_  = 0,
                    .count = 1,
                },
            .format = bucket.m_format,
        };
        if(m_view)
            bucket.view(
                gleam::textures::d2,
                view_params,
                std::weak_ptr<gleam::texture_2d_t>(m_view));
        else
            m_view = bucket.view(gleam::textures::d2, view_params);
    }

    void show_texture_copy(Proxy& e)
    {
        using typing::geometry::size_3d;

        if(!m_view)
        {
            auto& api = e.template subsystem<gfx::system>();
            m_view    = api.alloc_texture(gfx::textures::d2, PixDesc(), 1);
        }

        BitmapCache<V>& bitmap_cache   = e.template subsystem<BitmapCache<V>>();
        blam::bitm::image_t const* img = m_selected.image.mip;
        blam::magic_data_t         magic =
            (m_selected.tag->storage == blam::image_storage_t::internal &&
             m_map->map->version == blam::version_t::custom_edition)
                        ? m_map->magic
                        : bitmap_cache.bitm_magic;

        m_view->m_format = m_selected.image.fmt;
        m_view->alloc(
            size_3d<i32>{img->isize.x, img->isize.y, 1u}.convert<u32>());
        m_view->upload(
            img->data(magic), Veci2{}, Veci2{img->isize.x, img->isize.y});
    }

    void show_texture_imgui()
    {
        union
        {
            u32         hnd;
            ImTextureID ptr{nullptr};
        } tex_handle = {
            .hnd = m_view->m_handle,
        };

        ImGui::Text(
            "Tag: %s",
            m_selected.tag->to_name().to_string(m_map->magic).data());
        auto fmt = magic_enum::enum_name(m_selected.image.mip->format);
        ImGui::Text("Format: %.*s", static_cast<int>(fmt.size()), fmt.data());
        ImGui::Text(
            "Size: %ix%i",
            m_selected.image.mip->isize.x,
            m_selected.image.mip->isize.y);
        auto aspect_ratio = static_cast<f32>(m_selected.image.mip->isize.x) /
                            m_selected.image.mip->isize.y;
        auto uv1 = m_selected.image.offset;
        auto uv2 = uv1 + m_selected.image.scale;
        if(!m_view->m_features.image_copy)
            uv1 = {0, 0}, uv2 = {1, 1};
        ImGui::Image(
            tex_handle.ptr,
            ImVec2{512, 512 / aspect_ratio},
            ImVec2{uv1[0], uv1[1]},
            ImVec2{uv2[0], uv2[1]});
    }

    void list_textures(Proxy& e)
    {
        using cache_item_t           = std::pair<cache_id_t, BitmapItem>;
        BitmapCache<V>& bitmap_cache = e.template subsystem<BitmapCache<V>>();
        if(ImGui::BeginListBox("Textures", ImVec2{0, 512}))
        {
            for(cache_item_t item : bitmap_cache.m_cache)
            {
                auto item_name = item.second.tag->name.to_string(m_map->magic);
                if(item_name.empty())
                    continue;
                if(ImGui::Selectable(item_name.data()) &&
                   m_selected.tag != item.second.tag)
                {
                    m_selected = item.second;
                    m_updated  = true;
                }
            }
            ImGui::EndListBox();
        }
    }

    blam::map_container<V> const*      m_map{nullptr};
    BitmapItem                         m_selected{};
    std::shared_ptr<gfx::texture_2d_t> m_view;
    bool                               m_updated{false};
};
