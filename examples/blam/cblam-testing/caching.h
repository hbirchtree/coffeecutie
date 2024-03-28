#pragma once

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_font.h>
#include <blam/volta/blam_sound.h>

#include <coffee/comp_app/AppWrap>
#include <coffee/comp_app/gl_config.h>
#include <coffee/core/CApplication>
#include <coffee/core/CFiles>
#include <coffee/core/CProfiling>
#include <coffee/core/EventHandlers>
#include <coffee/core/Scene>

#include <peripherals/semantic/chunk_ops.h>
#include <peripherals/typing/enum/pixels/format_transform.h>

#include <coffee/graphics/apis/gleam/rhi.h>
#include <coffee/graphics/apis/gleam/rhi_compat.h>

#include <fmt_extensions/vector_types.h>

#include <coffee/core/CDebug>

#include "bitmap_cache.h"
#include "caching_item.h"
#include "data.h"
#include "data_cache.h"
#include "shader_cache.h"
#include "sound_cache.h"

using gfx_api = gleam::api;
using libc_types::byte_t;
using libc_types::u64;
using semantic::Bytes;
using semantic::BytesConst;
using semantic::Span;

using compo::time_point;

template<typename V>
struct ModelCache
    : DataCache<
          ModelItem<V>,
          std::tuple<u32, blam::mod2::mod2_lod>,
          blam::tagref_t const&,
          blam::mod2::mod2_lod>
    , compo::SubsystemBase
{
    using type    = ModelCache<V>;
    using Variant = typename std::conditional_t<
        std::is_same_v<V, blam::xbox_version_t>,
        blam::xbox_t,
        blam::grbx_t>;

    ModelCache(
        BitmapCache<V>& bitm_cache,
        ShaderCache<V>& shader_cache,
        gfx_api*        allocator)
        : bitm_cache(bitm_cache)
        , shader_cache(shader_cache)
        , allocator(allocator)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        version      = V::version_v;
        tags         = map.tags;
        index        = blam::tag_index_view(map);
        magic        = map.magic;
        vertex_magic = V::version_v == blam::version_t::xbox
                           ? map.magic
                           : tags->vertex_magic(magic);
        vert_ptr = 0, element_ptr = 0;
        this->evict_all();
    }

    blam::version_t             version;
    blam::tag_index_t<V> const* tags;
    blam::tag_index_view<V>     index;
    blam::magic_data_t          magic;
    blam::magic_data_t          vertex_magic;
    BitmapCache<V>&             bitm_cache;
    ShaderCache<V>&             shader_cache;
    gfx_api*                    allocator;

    Bytes vert_buffer, element_buffer;
    u32   vert_ptr, element_ptr;

    blam::mod2::header<V> const* get_header(blam::tagref_t const& mod2)
    {
        auto header_it = index.find(mod2);

        if(header_it == index.end())
            return nullptr;

        auto header =
            (*header_it).template data<blam::mod2::header<V>>(magic).value();

        return &header[0];
    }

    inline auto vertex_data(blam::mod2::part const& model)
    {
        return model.vertex_segment(*tags, magic).data(vertex_magic);
    }

    inline auto index_data(blam::mod2::part const& model)
    {
        return model.index_segment(*tags).data(vertex_magic);
    }

    virtual ModelItem<V> predict_impl(
        blam::tagref_t const& mod2, blam::mod2::mod2_lod lod) override;

    virtual void evict_impl() override
    {
        vert_ptr    = 0;
        element_ptr = 0;
    }

    virtual std::tuple<u32, blam::mod2::mod2_lod> get_id(
        blam::tagref_t const& tag, blam::mod2::mod2_lod lod) override
    {
        return std::make_tuple(tag.tag_id, lod);
    }

    ModelAssembly predict_regions(
        blam::tagref_t const& tag,
        blam::mod2::mod2_lod  max_lod = blam::mod2::lod_high_ext)
    {
        ModelAssembly assem  = {};
        auto          header = get_header(tag);

        if(!header)
            return {};

        assem.models.push_back(this->predict(tag, max_lod));

        //        for(auto const& region : header->regions.data(magic).value())
        //        {
        //            assem.models.emplace_back();
        //            auto& mod = assem.models.back();
        //            for(auto const& perm :
        //            region.permutations.data(magic).value())
        //            {
        //                auto const& lod = perm.meshindex_lod.at(max_lod);
        //                mod = this->predict(tag, lod);
        //                break;
        //            }
        //        }

        return assem;
    }
};

template<typename V>
struct BSPCache
    : DataCache<BSPItem, blam::bsp::info const*, blam::bsp::info const&>
    , compo::SubsystemBase
{
    using type  = BSPCache<V>;

    BSPCache(
        BitmapCache<V>& bitm_cache,
        ShaderCache<V>& shader_cache,
        SoundCache<V>&  sound_cache)
        : version(V::version_v)
        , bitm_cache(bitm_cache)
        , shader_cache(shader_cache)
        , sound_cache(sound_cache)
    {
    }

    inline void load_from(blam::map_container<V> const& map)
    {
        index    = blam::tag_index_view(map);
        magic    = map.magic;
        vert_ptr = 0, element_ptr = 0, light_ptr = 0, portal_ptr = reserved_debug_points,
        portal_color_ptr = 5;
        evict_all();
    }

    blam::version_t         version;
    BitmapCache<V>&         bitm_cache;
    ShaderCache<V>&         shader_cache;
    SoundCache<V>&          sound_cache;
    blam::tag_index_view<V> index;
    blam::magic_data_t      magic;

    Span<byte_t>           vert_buffer;
    Span<byte_t>           light_buffer;
    Span<blam::vert::face> element_buffer;

    Span<Vecf3> portal_buffer, portal_color_buffer;

    u32 vert_ptr, element_ptr, light_ptr, portal_ptr, portal_color_ptr;

    virtual BSPItem predict_impl(blam::bsp::info const& bsp) override;

    virtual blam::bsp::info const* get_id(blam::bsp::info const& bsp) override
    {
        return &bsp;
    }
};
