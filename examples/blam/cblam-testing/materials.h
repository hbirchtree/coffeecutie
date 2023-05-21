#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

namespace materials {

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf4;

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
    u32   reflection;
    u32   meta1{0};
    u32   meta2{0};
    /* 8 bytes of padding left */
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
    id    material; /* ID of the material */
    u32   flags;    /* Material-dependent flags */
    Vecf2 inputs1;
    Vecf4 inputs[11];
};

struct alignas(16) senv_micro
{
    map_data      maps[5];
    lightmap_data lightmap;
    material_data material;
};

static_assert(offsetof(senv_micro, lightmap) == 160);
static_assert(offsetof(senv_micro, material) == 192);
static_assert(sizeof(senv_micro) == 384);

struct alignas(16) light_properties
{
    Vecf4 light_direction;
    Vecf4 light_color;
};

struct alignas(16) fog_properties
{
    Vecf4 indoor_color;
    Vecf4 indoor_ambient;
    Vecf4 outdoor_color;
    Vecf4 outdoor_ambient;
    Vecf4 distances; /* xy = indoor range, zw = outdoor range */
};

struct alignas(16) world_data
{
    light_properties lighting[2];
    fog_properties fog;
};

static_assert(sizeof(world_data) == 144);

} // namespace materials
