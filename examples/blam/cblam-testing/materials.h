#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

namespace materials {

using libc_types::i32;
using libc_types::u32;
using libc_types::f32;
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

struct alignas(32) lightmap_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    u32   layer;
    id    material;
    u32   extension;
};

struct alignas(16) basic
{
    Vecf2 atlas_scale;
    Vecf2 uv_scale;
    Vecf2 atlas_offset;
    i32   source;
    u32   layer;
    f32   bias;
};

struct alignas(32) map_data
{
    Vecf2 atlas_scale;
    Vecf2 atlas_offset;
    Vecf2 uv_scale;
    u32   layer;
    f32   bias;
};

struct alignas(32) senv
{
    map_data      base;
    lightmap_data lightmap;
};

struct alignas(32) senv_micro
{
    map_data      base;
    map_data      micro;
    map_data      primary;
    map_data      secondary;
    lightmap_data lightmap;
};

} // namespace materials
