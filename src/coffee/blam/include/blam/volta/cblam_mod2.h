#pragma once

#include "cblam_structures.h"

namespace blam {

struct uvscale_t
{
    scalar u;
    scalar v;
};

namespace mod2 {

/*!
 * \brief Used as index for LOD levels of a mesh
 */
enum mod2_lod
{
    lod_low_ext,
    lod_low,
    lod_medium,
    lod_high,
    lod_high_ext,
};

struct region_permutation
{
    bl_string     name;
    u32           flags[8];
    Array<u16, 5> meshindex_lod;
    i16           reserved[7];
};

struct region
{
    bl_string_var<64>               name;
    reflexive_t<region_permutation> permutations;
};

enum class model_flags_t : u32
{
    blend_shared_normals = 0x1,
    local_nodes          = 0x2,
    ignore_skinning      = 0x3,
};

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

enum submesh_lod_cutoffs
{
    submesh_lod_lowest,
    submesh_lod_low,
    submesh_lod_medium,
    submesh_lod_high,
    submesh_lod_highest,
};

struct xbox_ref
{
    u32 unknown_;
    u32 offset;
    u32 unknown[2];

    inline auto vertices(u32 vert_count) const
    {
        reflexive_t<vert::vertex<vert::compressed>> out;
        out.count  = vert_count;
        out.offset = offset;
        out.zero   = 0;
        return out;
    }
};

struct submesh_header
{
    submesh_flags_t  flags;
    u16              shader_idx;
    u8               prev_idx;
    u8               next_idx;
    Array<u16, 2>    centroids;
    Array<scalar, 2> centroid_weights;
    Vecf3            centroid;

    u32 pad_[4];
    u32 sub_count;
    u32 pad__[4];
    u32 unknown_count;

    reflexive_t<vert::idx_t, xbox_variant> indices;

    u32 unknown_offset;
    u32 unknown_count2;
    u32 vert_count;
    u32 zero;
    u32 raw_offset_behavior;
    u32 vertex_ref_offset;

    inline decltype(tag_index_t::vertex_objects) vertex_segment(
        tag_index_t const& base) const
    {
        auto cpy = base.vertex_base();
        cpy.offset += vertex_ref_offset;
        cpy.count = vert_count;
        return cpy;
    }

    inline reflexive_t<xbox_ref> xbox_vertex_ref() const
    {
        reflexive_t<xbox_ref> out;
        out.count  = 1;
        out.offset = vertex_ref_offset;
        out.zero   = 0;
        return out;
    }

    /*!
     * \brief Gives the indices **for a triangle strip**
     * Trying to read these as normal triangles will be very bad
     * \param base
     */
    inline decltype(tag_index_t::index_objects) index_segment(
        tag_index_t const& base) const
    {
        auto cpy = base.index_base();
        cpy.offset += indices.offset;
        cpy.count = indices.count + 2;
        return cpy;
    }
    inline tag_index_t::index_array xbox_index_segment() const
    {
        return indices;
    }
};

template<typename Variant, bool = std::is_same<Variant, pc_variant>::value>
struct submesh_wrap_header
{
    submesh_header data;
    u32            unknown_2[7];

  private:
    constexpr void size_check()
    {
        static_assert(
            sizeof(submesh_wrap_header<pc_variant>) == 132, "Invalid size");
    }
};

template<typename Variant>
struct submesh_wrap_header<Variant, false>
{
    submesh_header data;

  private:
    constexpr void size_check()
    {
        static_assert(
            sizeof(submesh_wrap_header<xbox_variant>) == 104, "Invalid size");
    }
};

struct geometry_header
{
    u32 unknown[9];

    reflexive_t<char> meshes_;

    template<typename Variant>
    inline semantic::mem_chunk<submesh_wrap_header<Variant> const> meshes(
        magic_data_t const& magic) const
    {
        return meshes_.as<submesh_wrap_header<Variant>>().data(magic);
    }
};

struct marker
{
    bl_string           name;
    u32                 unknown[5];
    reflexive_t<byte_t> chunk;
};

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
    scalar    offset;
    scalar    unknown_[21];
};

/*!
 * \brief Header referenced by mod2 tags
 */
struct header
{
    model_flags_t zero1;
    u32           unknown1;
    u32           offset1;
    u32           offset2;
    u32           offset3;
    u32           offset4;
    u32           offset5;
    i16           lod_cutoff[5];
    i16           lod_nodecount[5];

    uvscale_t uvscale;

    u32 unknown2[29];

    reflexive_t<marker>          markers;
    reflexive_t<bone>            bones;
    reflexive_t<region>          regions;
    reflexive_t<geometry_header> geometries;
    reflexive_t<shader_desc>     shaders;
};

} // namespace mod2
} // namespace blam
