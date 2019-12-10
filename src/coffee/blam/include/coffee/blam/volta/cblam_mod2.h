#pragma once

#include "cblam_structures.h"

namespace blam {

struct uvscale_t
{
    scalar u;
    scalar v;
};

struct shader_desc
{
    u32 tag;
    u32 namePtr;
    u32 zero;
    u32 shaderTag;
    u32 unknown[4];
};

namespace mod2 {

/*!
 * \brief Used as index for LOD levels of a mesh
 */
enum mod2_lod
{
    mod2_load_low_ext,
    mod2_load_low,
    mod2_load_medium,
    mod2_load_high,
    mod2_load_high_ext,
};

struct report
{
    u32 index_offset_start;
    u32 index_offset_end;
    u32 vertex_offset_start;
    u32 vertex_offset_end;
};

struct region_permutation
{
    byte_t name[32];
    u32    flags[8];
    i16    meshindex_lod[5];
    i16    reserved[7];
};

struct region
{
    byte_t                          name[64];
    reflexive_t<region_permutation> permutations;
};

struct geometry_header
{
    u32                 unknown[9];
    reflexive_t<byte_t> mesh_headers;
};

struct marker
{
    byte_t              name[32];
    u32                 unknown[5];
    reflexive_t<byte_t> chunk;
};

/*!
 * \brief Header extracted from mod2 data. This is the first piece of data
 * referenced by the map, containing further references to parts of the model as
 * well as how to display it.
 */
struct header
{
    u32 zero1;
    u32 unknown1;
    u32 offset1;
    u32 offset2;
    u32 offset3;
    u32 offset4;
    u32 offset5;
    i16 lodcutoff_high_ext;
    i16 lodcutoff_high;
    i16 lodcutoff_med;
    i16 lodcutoff_low;
    i16 lodcutoff_low_ext;

    i16 nodecount_high_ext;
    i16 nodecount_high;
    i16 nodecount_med;
    i16 nodecount_low;
    i16 nodecount_low_ext;

    uvscale_t uvscale;

    u32 unknown2[29];

    reflexive_t<marker>          markers;
    reflexive_t<byte_t>          nodes;
    reflexive_t<region>          regions;
    reflexive_t<geometry_header> geometries;
    reflexive_t<shader_desc>     shaders;
};

struct bsp_header
{
    byte_t data[500];
    //    blam_tagref lightmaps_tag;
    //    uint32 unknown1[37];
    //    blam_reflexive<byte> shaders;
    //    blam_reflexive<byte> collision_bsp_header;
    //    blam_reflexive<byte> nodes;
    //    uint32 unknown2[6];
};

/*!
 * \brief Acquire the header of a model from the index item
 * \param item Item for which we acquire a model
 * \param map Map from which we extract a pointer
 * \param magic Magic number from the tag index
 * \return A valid pointer to the referenced model, or NULL if it is invalid
 */
extern const header* get_header(
    const tag_t* item, const file_header_t* map, i32 magic);

} // namespace mod2
} // namespace blam
