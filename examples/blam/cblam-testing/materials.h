#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

namespace materials {

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using typing::vector_types::Vecf2;

enum class id : u32
{
    undefined = 0,
    senv      = 1,
    swat      = 2,
    sgla      = 3,
    schi      = 4,
    scex      = 5,
    smet      = 6,
    sotr      = 7,
    spla      = 8,
    soso      = 9,
};

struct alignas(16) lightmap_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    u32   layer;
};

struct alignas(16) map_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    Vecf2 uv_scale;
    u32   layer;
    f32   bias;
};

struct alignas(16) material_data
{
    id  material; /* ID of the material */
    u32 flags;    /* Material-dependent flags */
};

struct alignas(16) senv_micro
{
    map_data      base;
    map_data      micro;
    map_data      primary;
    map_data      secondary;
    lightmap_data lightmap;
    material_data material;
    u32 padding[20];
};

static_assert(offsetof(senv_micro, lightmap) == 128);
static_assert(offsetof(senv_micro, material) == 160);
static_assert(sizeof(senv_micro) == 256);

} // namespace materials
