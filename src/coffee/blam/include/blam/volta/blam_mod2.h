#pragma once

#include "blam_base_types.h"
#include "blam_reference.h"
#include "blam_shaders.h"
#include "blam_tag_index.h"
#include "blam_vertex.h"

namespace blam::mod2 {

/*!
 * \brief Used as index for LOD levels of a mesh
 */
enum mod2_lod
{
    lod_high_ext,
    lod_high,
    lod_medium,
    lod_low,
    lod_low_ext,
};

enum class model_flags_t : u16
{
    blend_shared_normals = 0x1,
    local_nodes          = 0x2,
    ignore_skinning      = 0x3,
};

struct marker;
struct bone;
struct region;
template<typename Version>
struct geometry_header;
struct part;

template<typename T>
struct model_ptr
{
    i16 index;
};

static_assert(sizeof(model_ptr<void>) == 2);

/*!
 * \brief Not part of blam per se, but a wrapper to simplify the mod2 structures
 */
struct model_data_t
{
    mod2_lod                     lod;
    semantic::Span<const region> regions;
    semantic::Span<const marker> markers;
    semantic::Span<const bone>   bones;
    std::vector<part const*>     parts;
};

/*!
 * \brief Header referenced by mod2 tags
 */
template<typename V>
struct header
{
    model_flags_t      flags;
    u32                node_list_checksum;
    std::array<f32, 5> detail_cutoff;
    std::array<i16, 5> node_count;
    u32                unknown1[2];

    Vecf2 uvscale;

    u32 unknown2[29];

    reference<marker>              markers;
    reference<bone>                bones;
    reference<region>              regions;
    reference<geometry_header<V>>  geometries;
    reference<shader::shader_desc> shaders;

    /* `permutations`: per region, 4 bits each from region 0 up */
    inline std::optional<model_data_t> model_at(
        mod2_lod lod, map_ptr const& magic, u32 permutations = 0) const;

    inline u32 pick_permutations(
        map_ptr const& magic, u16 desired, u32 seed) const;
};

static_assert(offsetof(header<pc_version_t>, detail_cutoff) == 8);
static_assert(offsetof(header<pc_version_t>, node_count) == 28);
static_assert(offsetof(header<pc_version_t>, markers) == 172);
static_assert(sizeof(header<pc_version_t>) == 232);

struct marker
{
    struct instance_t
    {
        i8    region_idx;
        i8    permutation_idx;
        i8    node_idx;
        Vecf3 position;
        Vecf4 rotation;
    };

    bl_string             name;
    u32                   unknown[5];
    reference<instance_t> instances;
};

static_assert(sizeof(marker) == 64);
static_assert(sizeof(marker::instance_t) == 32);

struct bone
{
    static constexpr u16 invalid_bone = std::numeric_limits<u16>::max();

    bl_string name;
    u16       next_bone;
    u16       next_child;
    u16       parent;
    u16       unknown;
    Vecf3     translation;
    Quatf     rotation;
    f32       offset;
    f32       unknown_[8]; /* 0x48-0x64 */
    f32       inv_scale;
    Vecf3     inv_forward;
    Vecf3     inv_left;
    Vecf3     inv_up;
    Vecf3     inv_pos;
};

static_assert(sizeof(bone) == 156);

struct region_permutation;

struct region
{
    bl_string                     name;
    u32                           unknown[8];
    reference<region_permutation> permutations;
};

static_assert(sizeof(region) == 76);

enum class permutation_flags_t : u16
{
    none                      = 0x0,
    cannot_be_chosen_randomly = 0x1,
};

struct region_permutation
{
    bl_string           name;
    permutation_flags_t flags;
    u32                 unknown_1[7];
    /* Uses reverse order of LOD compared to mod2_lod!
     * References the geometries data in the header
     */
    std::array<u16, 5> meshindex_lod;
    reference<marker>  markers;
};

static_assert(sizeof(region_permutation) == 88);
static_assert(offsetof(region_permutation, meshindex_lod) == 64);

/* The number its name ends with, "head_rob-13", which placements ask for by
 * desired permutation; -1 without */
inline i32 permutation_number(region_permutation const& permutation)
{
    auto const name = permutation.name.str();
    auto const dash = name.rfind('-');
    if(dash == std::string_view::npos || dash + 1 == name.size())
        return -1;
    i32 number = 0;
    for(char c : name.substr(dash + 1))
    {
        if(c < '0' || c > '9')
            return -1;
        number = number * 10 + (c - '0');
    }
    return number;
}

/* Left to chance: not flagged against it, not a state like ~damaged, and
 * numbered below 100, where characters such as sgt_johnson-100 sit */
inline bool permutation_is_random(region_permutation const& permutation)
{
    return (static_cast<u16>(permutation.flags) &
            static_cast<u16>(permutation_flags_t::cannot_be_chosen_randomly)) == 0 &&
           !permutation.name.str().starts_with('~') &&
           permutation_number(permutation) < 100;
}

enum class submesh_flags_t : u32
{
    stripped_internal = 0x1,
    zoner             = 0x2,
};

enum submesh_centroid
{
    submesh_centroid_primary,
    submesh_centroid_second,
};

struct xbox_ref
{
    u32 unknown_;
    u32 offset;
    u32 unknown[2];

    inline auto vertices(u32 vert_count) const
    {
        /* Xbox mod2 vertices are mod2_vertex<compressed> (i16 texcoord + node
         * weights), NOT the weightless BSP vertex<compressed>. */
        vertex_reference<vert::mod2_vertex<vert::compressed>> out;
        out.count  = vert_count;
        out.offset = offset;
        return out;
    }
};

struct part
{
    submesh_flags_t    flags;
    u16                shader_idx;
    u8                 prev_idx;
    u8                 next_idx;
    std::array<u16, 2> centroids;
    std::array<f32, 2> centroid_weights;
    Vecf3              centroid;

    reference<vert::uncompressed> uncompressed_vertices;
    reference<vert::compressed>   compressed_vertices;

    reference<u32> triangles;
    u32            pad_[1];

    vertex_reference<vert::idx_t, xbox_t> indices;

    u32 pad2;

    vert::vertex_type_t vertex_type;

    union
    {
        reference<vert::compressed> vertices;

        struct
        {
            u32 vert_count;
            u32 zero;
            u32 raw_offset_behavior;
            u32 vertex_ref_offset;
        };
    };

    template<typename V>
    inline auto vertex_segment(
        tag_index_t<V> const& base, map_ptr const& magic) const
    {
        if constexpr(std::is_same_v<V, xbox_version_t>)
        {
            reference<xbox_ref> out;
            out.count  = to_le<u32>(1);
            out.offset = vertex_ref_offset;
            return out.data(magic, blam::single_value)
                .value()
                ->vertices(vert_count);
        } else
        {
            auto cpy = base.vertex_base();
            // offsets/counts are little-endian; do arithmetic in host order and
            // re-encode so reference::data() un-swaps correctly.
            cpy.offset =
                to_le(from_le(cpy.offset) + from_le(vertex_ref_offset));
            cpy.count = vert_count; // already little-endian
            return cpy;
        }
    }

    template<typename V>
    /*!
     * \brief Gives the indices **for a triangle strip**
     * Trying to read these as normal triangles will be very bad
     * \param base
     */
    inline auto index_segment(tag_index_t<V> const& base) const
    {
        if constexpr(std::is_same_v<V, xbox_version_t>)
        {
            auto cpy  = indices;
            cpy.count = to_le(from_le(indices.count) + 2);
            return cpy;
        } else
        {
            auto cpy   = base.index_base();
            cpy.offset = to_le(from_le(cpy.offset) + from_le(indices.offset));
            cpy.count  = to_le(from_le(indices.count) + 2);
            return cpy;
        }
    }
};

static_assert(offsetof(part, uncompressed_vertices) == 32);
static_assert(offsetof(part, compressed_vertices) == 44);
static_assert(offsetof(part, indices) == 72);

template<typename Variant>
requires is_game_version<Variant>
/**
 * @brief This is only used to adjust padding between Xbox and PC
 */
struct part_wrap_header
{
    static constexpr size_t padding_size =
        std::is_same_v<Variant, xbox_t> ? 0 : 28;

    part data;
    u8   unknown_2[padding_size];
};

static_assert(sizeof(part_wrap_header<xbox_t>) == 104);
static_assert(sizeof(part_wrap_header<grbx_t>) == 132);

template<typename Version>
struct geometry_header
{
    u32 unknown[9];

    reference<part_wrap_header<Version>> meshes_;

    inline auto meshes(map_ptr const& magic) const
    {
        if(auto out = meshes_.data(magic); out.has_value())
            return out.value();
        else
            return typename decltype(meshes_)::span_type();
    }
};

static_assert(sizeof(geometry_header<pc_version_t>) == 48);

template<typename V>
inline u32 header<V>::pick_permutations(
    map_ptr const& magic, u16 desired, u32 seed) const
{
    auto regions_ptr = regions.data(magic);
    if(!regions_ptr.has_value())
        return 0;
    u32 out = 0;
    u32 r   = 0;
    for(region const& reg : regions_ptr.value())
    {
        if(r >= 8)
            break;
        auto perms = reg.permutations.data(magic);
        if(!perms.has_value() || perms.value().empty())
        {
            r++;
            continue;
        }
        auto const& list   = perms.value();
        u32         choice = 0;
        bool        found  = false;
        for(u32 i = 0; i < list.size() && i < 16 && desired; i++)
            if(permutation_number(list[i]) == static_cast<i32>(desired))
            {
                choice = i;
                found  = true;
            }
        if(!found)
        {
            u32 candidates = 0;
            for(u32 i = 0; i < list.size() && i < 16; i++)
                candidates += permutation_is_random(list[i]);
            u32 pick = candidates ? ((seed ^ (r * 0x9E3779B9u)) * 0x85EBCA6Bu >> 16) % candidates : 0;
            for(u32 i = 0; i < list.size() && i < 16; i++)
                if(permutation_is_random(list[i]) && pick-- == 0)
                {
                    choice = i;
                    break;
                }
        }
        out |= choice << (4 * r);
        r++;
    }
    return out;
}

template<typename V>
inline std::optional<model_data_t> header<V>::model_at(
    mod2_lod lod, map_ptr const& magic, u32 permutations) const
{
    auto regions_ptr = regions.data(magic);
    if(!regions_ptr.has_value())
        return std::nullopt;
    auto geom_ptr = geometries.data(magic);
    if(!geom_ptr.has_value())
        return std::nullopt;
    semantic::Span<const geometry_header<V>> geometry = geom_ptr.value();
    semantic::Span<const region>             regions_ = regions_ptr.value();
    std::vector<part const*>                 meshes;
    u32 r = 0;
    for(region const& reg : regions_)
    {
        auto list   = reg.permutations.data(magic).value();
        u32  chosen = r < 8 ? (permutations >> (4 * r)) & 0xF : 0;
        r++;
        if(chosen >= list.size())
            chosen = 0;
        for(region_permutation const& per : list.subspan(chosen))
        {
            auto perm_idx = from_le(per.meshindex_lod.at(4u - lod));
            auto parts    = geometry[perm_idx].meshes(magic);
            for(part_wrap_header<V> const& part : parts)
            {
                if(!stl_types::any_of(
                       from_le(part.data.vertex_type),
                       vert::vertex_type_t::mod2_compressed_vertex,
                       vert::vertex_type_t::mod2_uncompressed_vertex,
                       vert::vertex_type_t::mod2_gx_vertex))
                    continue;
                meshes.push_back(&part.data);
            }
            break;
        }
    }
    // TODO: Add association between mesh and region?
    return model_data_t{
        .lod     = lod,
        .regions = regions_,
        .markers = markers.data(magic).value(),
        .bones   = bones.data(magic).value(),
        .parts   = std::move(meshes),
    };
}

} // namespace blam::mod2
