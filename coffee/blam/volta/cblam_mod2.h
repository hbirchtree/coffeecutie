#ifndef CBLAM_MOD2
#define CBLAM_MOD2

#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

/*!
 * \brief Used as index for LOD levels of a mesh
 */
enum blam_mod2_lod
{
    blam_mod2_load_low_ext,
    blam_mod2_load_low,
    blam_mod2_load_medium,
    blam_mod2_load_high,
    blam_mod2_load_high_ext,
};

struct blam_uvscale
{
    scalar u;
    scalar v;
};

struct blam_mod2_report
{
    uint32 index_offset_start;
    uint32 index_offset_end;
    uint32 vertex_offset_start;
    uint32 vertex_offset_end;
};

struct blam_shader_desc
{
    uint32 tag;
    uint32 namePtr;
    uint32 zero;
    uint32 shaderTag;
    uint32 unknown[4];
};

struct blam_mod2_region_permutation
{
    byte name[32];
    uint32 flags[8];
    int16 meshindex_lod[5];
    int16 reserved[7];
};

struct blam_mod2_region
{
    byte name[64];
    blam_reflexive<blam_mod2_region_permutation> permutations;
};

struct blam_mod2_geometry_header
{
    uint32 unknown[9];
    blam_reflexive<byte> mesh_headers;
};

struct blam_mod2_marker
{
    byte name[32];
    uint32 unknown[5];
    blam_reflexive<byte> chunk;
};

/*!
 * \brief Header extracted from mod2 data. This is the first piece of data referenced by the map, containing further references to parts of the model as well as how to display it.
 */
struct blam_mod2_header
{
    uint32 zero1;
    uint32 unknown1;
    uint32 offset1;
    uint32 offset2;
    uint32 offset3;
    uint32 offset4;
    uint32 offset5;
    int16 lodcutoff_high_ext;
    int16 lodcutoff_high;
    int16 lodcutoff_med;
    int16 lodcutoff_low;
    int16 lodcutoff_low_ext;

    int16 nodecount_high_ext;
    int16 nodecount_high;
    int16 nodecount_med;
    int16 nodecount_low;
    int16 nodecount_low_ext;

    blam_uvscale uvscale;

    uint32 unknown2[29];

    blam_reflexive<blam_mod2_marker> markers;
    blam_reflexive<byte> nodes;
    blam_reflexive<blam_mod2_region> regions;
    blam_reflexive<blam_mod2_geometry_header> geometries;
    blam_reflexive<blam_shader_desc> shaders;
};

/*!
 * \brief The blam_mod2_bsp_header struct
 */
struct blam_mod2_bsp_header
{
    byte data[500];
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
extern const blam_mod2_header* blam_mod2_get_header(
        const blam_index_item* item,
        const blam_file_header* map,
        int32 magic);

}
}

#endif
