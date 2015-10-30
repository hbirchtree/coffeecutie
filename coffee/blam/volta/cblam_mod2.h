#ifndef CBLAM_MOD2
#define CBLAM_MOD2

#include "cblam_structures.h"

namespace Coffee{
namespace CBlam{

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

    blam_reflexive markers;
    blam_reflexive nodes;
    blam_reflexive regions;
    blam_reflexive geometries;
    blam_reflexive shaders;
};

struct blam_shader_desc
{
    uint32 tag;
    uint32 namePtr;
    uint32 zero;
    uint32 shaderTag;
    uint32 unknown[4];
};

struct blam_mod2_region
{
    byte name[64];
    blam_reflexive permutations;
};

struct blam_mod2_region_permutation
{
    byte name[32];
    uint32 flags[8];
    int16 meshindex_lod[5];
    int16 reserved[7];
};

}
}

#endif
