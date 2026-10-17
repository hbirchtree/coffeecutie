#pragma once

#include "../volta/blam_swizzle.h"
#include "bitm.h"
#include "geometry.h"
#include "map.h"
#include "vista.h"

#include <array>
#include <cstring>
#include <optional>
#include <vector>

namespace blam::dimeter::ltmp {

/*!
 * scenario_structure_lightmap (ltmp): per BSP, lightmap groups holding a
 * bitmap group (one P8 image per cluster), 256-colour palettes, and a
 * geometry section per cluster carrying the lightmap texcoords in the same
 * vertex order as the sbsp cluster. Layout from Assembly's Halo 2 ltmp.xml.
 */

struct alignas(4) palette_t
{
    std::array<u32, 256> colors; /*!< R, G, B, A bytes */
};

static_assert(sizeof(palette_t) == 0x400);

/*! Which image of the bitmap group and which palette a cluster uses */
struct alignas(2) render_info_t
{
    block_index<bitm::image_t> bitmap;
    block_index<palette_t, i8> palette;
    i8                         unknown_0;
};

static_assert(sizeof(render_info_t) == 4);

/*! Compression info with the secondary (lightmap) texcoord bounds */
struct alignas(4) bounds_t
{
    f32 x_min, x_max;
    f32 y_min, y_max;
    f32 z_min, z_max;
    f32 u_min, u_max;
    f32 v_min, v_max;
    f32 u2_min, u2_max;
    f32 v2_min, v2_max;
};

static_assert(sizeof(bounds_t) == 0x38);

/*! Same shape as an sbsp cluster's section, 0x54 bytes */
struct alignas(4) cluster_t : geo::bsp_section_common_t
{
    u8 unknown_3[20];

    inline result<Span<bounds_t const>, error_msg> lightmap_bounds(
        bsp_magic const& magic) const
    {
        return reinterpret_cast<bsp_block<bounds_t> const&>(bounds).data(magic);
    }
};

static_assert(sizeof(cluster_t) == 0x54);

/*! Per-vertex lighting stream for instances and scenery lit per vertex;
 * the same raw-section shape as a cluster's geometry */
struct alignas(4) geometry_bucket_t
{
    u16                        flags; /* TODO: bits unknown */
    u8                         unknown_0[10];
    raw_ref                    data;
    i32                        data_size_raw;
    i32                        header_size_raw;
    i32                        resource_data_size_raw;
    bsp_block<geo::resource_t> resources;
    tag_id<>                   owner_tag;
    i16                        owner_tag_section_offset_raw;
    i16                        unknown_1;
    u8                         unknown_2[0x38 - 0x2C];

    inline i32 data_size() const
    {
        return from_le(data_size_raw);
    }

    inline i32 base_address() const
    {
        return from_le(header_size_raw) + 8;
    }
};

static_assert(offsetof(geometry_bucket_t, resources) == 0x1C);
static_assert(sizeof(geometry_bucket_t) == 0x38);

struct alignas(2) section_offset_t
{
    i16 raw;

    inline i16 value() const
    {
        return from_le(raw);
    }
};

/*! Which bucket an instance or scenery object's sections are lit from, and
 * where each section's vertices start in it */
struct alignas(4) bucket_ref_t
{
    i16                            flags; /* TODO: bits unknown */
    block_index<geometry_bucket_t> bucket;
    bsp_block<section_offset_t>    section_offsets;
};

static_assert(sizeof(bucket_ref_t) == 0xC);

/*! Identifies the scenario placement a scenery lighting entry belongs to */
struct alignas(4) scenery_object_info_t
{
    datum_index                           unique_id;
    block_index<geo::structure_bsp_ref_t> origin_bsp;
    u8                                    type;
    u8                                    source;
    i32                                   render_model_checksum_raw;
};

static_assert(sizeof(scenery_object_info_t) == 0xC);

/*! Light at a sample point for objects: order-2 spherical harmonics per
 * channel, and the dominant incoming direction */
struct alignas(4) lighting_environment_t
{
    Vecf3              sample_point;
    std::array<f32, 9> red;
    std::array<f32, 9> green;
    std::array<f32, 9> blue;
    Vecf3              mean_direction;
    Vecf3              intensity;
    i32                specular_bitmap;
    Vecf3              rotation_axis;
    f32                rotation_speed;
    Vecf3              bump_direction;
    Vecf3              tint;
    u16                procedural_override;
    u16                flags; /* TODO: flags */
    Vecf3              procedural_0;
    Vecf3              procedural_1;
    f32                procedural_w;
};

static_assert(offsetof(lighting_environment_t, mean_direction) == 0x78);
static_assert(sizeof(lighting_environment_t) == 0xDC);

struct alignas(4) group_t
{
    u16                              type;  /* TODO: enumerate */
    u16                              flags; /* TODO: bits unknown */
    i32                              structure_checksum;
    bsp_block<palette_t>             section_palette;
    bsp_block<palette_t>             writable_palettes;
    tag_reference<tag_class_t::bitm> bitmap_group;
    bsp_block<cluster_t>             clusters;
    bsp_block<render_info_t>         cluster_render_info;
    bsp_block<cluster_t>             poop_definitions; /* instanced geometry */
    bsp_block<lighting_environment_t> lighting_environments;
    bsp_block<geometry_bucket_t>     geometry_buckets;
    bsp_block<render_info_t>         instance_render_info;
    bsp_block<bucket_ref_t>          instance_bucket_refs;
    bsp_block<scenery_object_info_t> scenery_object_info;
    bsp_block<bucket_ref_t>          scenery_bucket_refs;
};

static_assert(offsetof(group_t, bitmap_group) == 0x18);
static_assert(offsetof(group_t, cluster_render_info) == 0x28);
static_assert(sizeof(group_t) == 0x68);

struct alignas(4) header_t
{
    u8                 unknown_0[0x80];
    bsp_block<group_t> groups;
};

static_assert(offsetof(header_t, groups) == 0x80);

/*! A decoded lightmap: RGBA8, one byte per channel */
struct image_t
{
    u32             width{0};
    u32             height{0};
    std::vector<u8> rgba;
};

/*! P8 indices (Morton-swizzled on Xbox) through a BGRA palette */
inline std::optional<image_t> decode(
    raw_pool const& pool, bitm::image_t const& img, palette_t const& palette)
{
    auto const format = from_le(img.format);
    bool const argb   = format == blam::bitm::format_t::ARGB8 ||
                      format == blam::bitm::format_t::XRGB8;
    if(from_le(img.type) != blam::bitm::type_t::tex_2d ||
       (format != blam::bitm::format_t::P8_flat && !argb))
        return std::nullopt;
    u32 width  = static_cast<u32>(from_le(img.width));
    u32 height = static_cast<u32>(from_le(img.height));
    u32 base   = width * height;
    if(!base)
        return std::nullopt;
    for(int lod = 0; lod < 3; lod++)
    {
        u32 lod_size = from_le(img.lod_size[lod]);
        if(!lod_size)
            continue;
        auto raw = pool.resolve(img.lod_offset[lod], lod_size);
        if(!raw)
            continue;
        std::optional<std::vector<u8>> inflated;
        if(vista::is_zlib(*raw))
            inflated = vista::inflate(*raw);
        Span<const u8> in =
            inflated
                ? Span<const u8>(*inflated)
                : Span<const u8>(
                      reinterpret_cast<u8 const*>(raw->data()), raw->size());
        /* Vista: already colours, stored B, G, R, A */
        if(argb)
        {
            if(in.size() < base * 4)
                continue;
            image_t out{.width = width, .height = height};
            out.rgba.resize(static_cast<size_t>(base) * 4);
            for(u32 i = 0; i < base; i++)
            {
                out.rgba[i * 4 + 0] = in[i * 4 + 2];
                out.rgba[i * 4 + 1] = in[i * 4 + 1];
                out.rgba[i * 4 + 2] = in[i * 4 + 0];
                out.rgba[i * 4 + 3] = 0xFF;
            }
            return out;
        }
        if(in.size() < base)
            continue;
        in = in.subspan(0, base);
        std::vector<u8> indices(base);
        if(!img.swizzled() ||
           !swizzle::deswizzle_bytes(
               in, Span<u8>(indices.data(), base), width, height, 1))
            std::memcpy(indices.data(), in.data(), base);
        image_t out{.width = width, .height = height};
        out.rgba.resize(static_cast<size_t>(base) * 4);
        for(u32 i = 0; i < base; i++)
        {
            u32 color           = from_le(palette.colors[indices[i]]);
            out.rgba[i * 4 + 0] = static_cast<u8>(color >> 16);
            out.rgba[i * 4 + 1] = static_cast<u8>(color >> 8);
            out.rgba[i * 4 + 2] = static_cast<u8>(color);
            out.rgba[i * 4 + 3] = 0xFF;
        }
        return out;
    }
    return std::nullopt;
}

/*! The ltmp body of a BSP region, where its header points */
inline header_t const* from_region(
    geo::bsp_region_header_t const& region, bsp_magic const& magic)
{
    return magic.as<header_t>(region.lightmap_pointer());
}

} // namespace blam::dimeter::ltmp

namespace blam::dimeter {

/*! A structure BSP and its lightmap, wherever the cache keeps them: Xbox
 * in a region of their own with its own virtual base, Vista as ordinary
 * tags in the tag heap */
struct bsp_location_t
{
    geo::bsp_header_t const* body{nullptr};
    bsp_magic                magic{};
    ltmp::header_t const*    lightmap{nullptr};
};

template<typename V>
inline bsp_location_t locate_bsp(
    map_container<V> const& map, geo::structure_bsp_ref_t const& ref)
{
    bsp_location_t out;
    auto const&    tag_magic = map.magic();
    if(ref.meta_address() != 0)
    {
        out.body  = ref.body(tag_magic);
        out.magic = ref.region_magic(tag_magic);
        if(auto const* region = ref.region(tag_magic))
            out.lightmap = ltmp::from_region(*region, out.magic);
        return out;
    }
    if(auto const* bsp_tag = map.tag_at(ref.bsp.tag_id))
        out.body = bsp_tag->template data<geo::bsp_header_t>(tag_magic);
    out.magic = bsp_magic{
        .base        = tag_magic.base,
        .max_size    = tag_magic.max_size,
        .meta_offset = tag_magic.meta_offset,
        .mask        = tag_magic.mask,
    };
    if(auto const* ltmp_tag = map.tag_at(ref.lightmap.tag_id))
        out.lightmap = ltmp_tag->template data<ltmp::header_t>(tag_magic);
    return out;
}

} // namespace blam::dimeter
