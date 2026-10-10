#pragma once

#include "caching.h"
#include "caching_item.h"

#include <blam/volta/blam_bitm.h>
#include <blam/volta/blam_bsp_structures.h>
#include <blam/volta/blam_scenario.h>
#include <glm/geometric.hpp>

/* Detail objects (grass) of the resident BSP section, built each frame into
 * sprites facing the viewer within the collections' fade distance */
template<typename V>
struct DetailObjects
{
    using u8x4 = typing::vectors::tvector<libc_types::u8, 4>;

    struct vertex_t
    {
        Vecf3 position;
        Vecf2 tex_coord;
        u8x4  color;
    };

    struct group_t
    {
        generation_idx_t      bitmap;
        libc_types::u32       source; /*!< format << 24 | atlas layer */
        std::vector<vertex_t> vertices;
    };

    struct sprite_t
    {
        Vecf4           uv;   /*!< left, top, right, bottom in the atlas */
        Vecf2           size; /*!< world units */
        libc_types::u32 group;
    };

    struct type_t
    {
        libc_types::f32 near_fade, far_fade;
        Vecf3           min_color, max_color;
        libc_types::u32 first_sprite, sprite_count;
    };

    struct collection_t
    {
        std::vector<type_t> types;
        bool                viewer_facing{true};
    };

    struct instance_t
    {
        Vecf3           position;
        libc_types::u16 color;
        libc_types::u8  collection;
        libc_types::u8  data;
    };

    struct cell_t
    {
        Vecf3           min, max;
        libc_types::u32 first, count;
    };

    std::vector<collection_t> collections;
    std::vector<sprite_t>     sprites;
    std::vector<instance_t>   instances;
    std::vector<cell_t>       cells;
    std::vector<group_t>      groups;
    libc_types::f32           max_far_fade{0.f};

    std::tuple<blam::bsp::info const*, libc_types::u32, libc_types::u32> key{};

    /* Rebuilds when the resident section or either cache changed */
    void update(
        BSPCache<V>&                  bsp_cache,
        BitmapCache<V>&               bitm_cache,
        blam::scn::scenario<V> const* scenario)
    {
        using namespace libc_types;

        blam::bsp::info const* info = nullptr;
        if(scenario && bsp_cache.active_section >= 0 &&
           static_cast<size_t>(bsp_cache.active_section) <
               bsp_cache.sections.size())
            info = bsp_cache.sections[bsp_cache.active_section];
        auto const next =
            std::make_tuple(info, bsp_cache.generation, bitm_cache.generation);
        if(next == key)
            return;
        key = next;

        collections.clear();
        sprites.clear();
        instances.clear();
        cells.clear();
        groups.clear();
        max_far_fade = 0.f;
        if(!info)
            return;

        auto const& magic = bsp_cache.magic;
        auto const& index = bsp_cache.index;

        std::vector<f32> z_offsets;
        if(auto palette = scenario->detail_object_collection.data(magic))
            for(auto const& entry : palette.value())
            {
                collections.emplace_back();
                z_offsets.push_back(0.f);
                auto const& ref =
                    static_cast<blam::tagref_t const&>(entry.reference);
                auto tag = index.tag_of(ref);
                if(!ref.valid() || !tag.has_value())
                    continue;
                auto dobc =
                    (*tag)->template data<blam::scn::detail_object_collection>(
                        magic);
                if(!dobc.has_value() || !dobc.value())
                    continue;
                load_collection(
                    index,
                    magic,
                    bitm_cache,
                    *dobc.value(),
                    collections.back());
                z_offsets.back() = dobc.value()->global_z_offset;
            }

        auto bsp_magic = info->bsp_magic(magic);
        auto header    = info->to_bsp(bsp_magic).to_header().data(
            bsp_magic, blam::single_value);
        if(header.has_error())
            return;
        auto data = header.value()->detail_objects.data(bsp_magic);
        if(!data.has_value())
            return;

        for(auto const& det : data.value())
        {
            auto cells_  = det.cells.data(bsp_magic);
            auto inst_   = det.instances.data(bsp_magic);
            auto counts_ = det.counts.data(bsp_magic);
            auto zref_   = det.z_references.data(bsp_magic);
            if(!cells_.has_value() || !inst_.has_value() ||
               !counts_.has_value() || !zref_.has_value())
                continue;
            auto const& src    = inst_.value();
            auto const& counts = counts_.value();
            auto const& zrefs  = zref_.value();
            for(auto const& cell : cells_.value())
            {
                cell_t out{
                    .min   = Vecf3(std::numeric_limits<f32>::max()),
                    .max   = Vecf3(std::numeric_limits<f32>::lowest()),
                    .first = static_cast<u32>(instances.size()),
                };
                u32 const layers = cell.layers;
                u32       next   = cell.first_instance;
                u32       slot   = cell.first_count;
                for(u32 layer = 0; layer < 32; ++layer)
                {
                    if(!((layers >> layer) & 1))
                        continue;
                    if(slot >= counts.size() || slot >= zrefs.size())
                        break;
                    u32 const count = counts[slot];
                    for(u32 i = next;
                        layer < collections.size() &&
                        i < std::min<size_t>(next + count, src.size());
                        ++i)
                    {
                        auto const pos = blam::bsp::detail_object::position(
                                             cell, zrefs[slot], src[i]) +
                                         Vecf3(0, 0, z_offsets[layer]);
                        out.min = glm::min(out.min, pos);
                        out.max = glm::max(out.max, pos);
                        instances.push_back({
                            .position   = pos,
                            .color      = src[i].color,
                            .collection = static_cast<u8>(layer),
                            .data       = src[i].data,
                        });
                    }
                    next += count;
                    ++slot;
                }
                out.count = static_cast<u32>(instances.size()) - out.first;
                if(out.count)
                    cells.push_back(out);
            }
        }
    }

    /* Sprites of every viewer-near instance, into groups[].vertices */
    void build(Vecf3 const& camera)
    {
        using namespace libc_types;

        for(auto& group : groups)
            group.vertices.clear();

        for(auto const& cell : cells)
        {
            if(glm::distance(glm::clamp(camera, cell.min, cell.max), camera) >
               max_far_fade)
                continue;
            for(u32 i = cell.first; i < cell.first + cell.count; ++i)
                add_sprite(camera, i);
        }
    }

  private:
    void load_collection(
        blam::tag_index_view<V> const&             index,
        blam::map_ptr const&                       magic,
        BitmapCache<V>&                            bitm_cache,
        blam::scn::detail_object_collection const& dobc,
        collection_t&                              out)
    {
        using namespace libc_types;

        out.viewer_facing = dobc.collection_type !=
                            blam::scn::detail_object_collection::
                                collection_type_t::screen_facing;

        auto const& plate_ref =
            static_cast<blam::tagref_t const&>(dobc.sprite_plate);
        auto plate_tag = index.tag_of(plate_ref);
        auto types     = dobc.types.data(magic);
        if(!plate_tag.has_value() || !types.has_value())
            return;
        auto plate = (*plate_tag)->template data<blam::bitm::header_t>(magic);
        if(!plate.has_value() || !plate.value())
            return;
        auto sequences = plate.value()->sequences.data(magic);
        if(!sequences.has_value())
            return;

        for(auto const& type : types.value())
        {
            type_t& t = out.types.emplace_back(
                type_t{
                    .near_fade    = type.near_fade,
                    .far_fade     = type.far_fade,
                    .min_color    = type.min_color,
                    .max_color    = type.max_color,
                    .first_sprite = static_cast<u32>(sprites.size()),
                    .sprite_count = 0,
                });
            max_far_fade = std::max(max_far_fade, t.far_fade);
            if(type.sequence >= sequences.value().size())
                continue;
            auto seq_sprites =
                sequences.value()[type.sequence].sprites.data(magic);
            if(!seq_sprites.has_value())
                continue;
            auto const& list = seq_sprites.value();
            for(u32 s = type.first_sprite;
                s < std::min<size_t>(
                        type.first_sprite + type.sprite_count, list.size());
                ++s)
            {
                auto const& sprite = list[s];
                auto        bitmap = bitm_cache.predict(
                    plate_ref, static_cast<i16>(sprite.bitmap_index));

                struct
                {
                    Vecf2 atlas_offset, atlas_scale;
                    u32   layer;
                } atlas{};

                auto const* item = bitm_cache.assign_atlas_data(atlas, bitmap);
                if(!item)
                    continue;
                Vecf2 const image(
                    item->image.mip->isize.x, item->image.mip->isize.y);
                Vecf4 const rect(
                    sprite.left, sprite.top, sprite.right, sprite.bottom);
                sprites.push_back({
                    .uv = Vecf4(
                        atlas.atlas_offset +
                            atlas.atlas_scale * Vecf2(rect.x, rect.y),
                        atlas.atlas_offset +
                            atlas.atlas_scale * Vecf2(rect.z, rect.w)),
                    .size = Vecf2(rect.z - rect.x, rect.w - rect.y) * image *
                            type.size,
                    .group = group_of(bitmap, atlas.layer),
                });
                t.sprite_count++;
            }
        }
    }

    libc_types::u32 group_of(generation_idx_t bitmap, libc_types::u32 source)
    {
        for(auto const& [i, group] : stl_types::const_enumerate(groups))
            if(group.bitmap == bitmap)
                return static_cast<libc_types::u32>(i);
        groups.push_back({.bitmap = bitmap, .source = source});
        return static_cast<libc_types::u32>(groups.size() - 1);
    }

    void add_sprite(Vecf3 const& camera, libc_types::u32 idx)
    {
        using namespace libc_types;

        instance_t const&   inst = instances[idx];
        collection_t const& col  = collections[inst.collection];

        /* The data byte counts sprites across the collection's types */
        u32           sprite = inst.data;
        type_t const* type   = nullptr;
        for(auto const& t : col.types)
        {
            if(sprite < t.sprite_count)
            {
                type = &t;
                break;
            }
            sprite -= t.sprite_count;
        }
        if(!type)
            return;

        Vecf3 const to_camera = camera - inst.position;
        f32 const   distance  = glm::length(to_camera);
        if(distance >= type->far_fade || distance < 1e-3f)
            return;
        /* Fading out shrinks the sprite into the ground */
        f32 const scale =
            1.f - glm::clamp(
                      (distance - type->near_fade) /
                          std::max(type->far_fade - type->near_fade, 1e-3f),
                      0.f,
                      1.f);

        sprite_t const& s     = sprites[type->first_sprite + sprite];
        Vecf3 const     dir   = to_camera / distance;
        Vecf3           right = glm::cross(Vecf3(0, 0, 1), dir);
        right =
            glm::length(right) > 1e-3f ? glm::normalize(right) : Vecf3(1, 0, 0);
        Vecf3 const up =
            col.viewer_facing ? Vecf3(0, 0, 1) : glm::cross(dir, right);
        Vecf3 const half_w = right * (s.size.x * scale * .5f);
        Vecf3 const height = up * (s.size.y * scale);

        /* Per-instance tint between the type's colours; lighting is baked */
        f32 const mix =
            static_cast<f32>((idx * 2654435761u) >> 16 & 0xFFFF) / 65535.f;
        u16 const   c    = inst.color;
        Vecf3 const tint = Vecf3(
                               ((c >> 11) & 31) / 31.f,
                               ((c >> 5) & 63) / 63.f,
                               (c & 31) / 31.f) *
                           glm::mix(type->min_color, type->max_color, mix);
        u8x4 const color(
            static_cast<u8>(glm::clamp(tint.x, 0.f, 1.f) * 255.f),
            static_cast<u8>(glm::clamp(tint.y, 0.f, 1.f) * 255.f),
            static_cast<u8>(glm::clamp(tint.z, 0.f, 1.f) * 255.f),
            255);

        Vecf3 const bl  = inst.position - half_w;
        Vecf3 const br  = inst.position + half_w;
        Vecf4 const uv  = s.uv;
        auto&       out = groups[s.group].vertices;
        out.insert(
            out.end(),
            {
                {bl, Vecf2(uv.x, uv.w), color},
                {br, Vecf2(uv.z, uv.w), color},
                {br + height, Vecf2(uv.z, uv.y), color},
                {bl, Vecf2(uv.x, uv.w), color},
                {br + height, Vecf2(uv.z, uv.y), color},
                {bl + height, Vecf2(uv.x, uv.y), color},
            });
    }
};
