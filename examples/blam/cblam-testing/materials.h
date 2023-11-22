#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <blam/volta/blam_shaders.h>

#include <stdexcept>

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

struct alignas(16) shader_data
{
    map_data      maps[5];
    lightmap_data lightmap;
    material_data material;
};

static_assert(offsetof(shader_data, lightmap) == 160);
static_assert(offsetof(shader_data, material) == 192);
static_assert(sizeof(shader_data) == 384);

struct alignas(16) transparent_data
{
    using input_t       = blam::shader::color_input;
    using output_t      = blam::shader::color_output;
    using output_func_t = blam::shader::color_output_function;
    using mapping_t     = blam::shader::shader_transparent::mapping_t;

    PACKEDSTRUCT(stage_t)
    {
        /* 4 * 8 bits */
        input_t   color_a_input : 5;
        mapping_t color_a_mapping : 3;
        input_t   color_b_input : 5;
        mapping_t color_b_mapping : 3;
        input_t   color_c_input : 5;
        mapping_t color_c_mapping : 3;
        input_t   color_d_input : 5;
        mapping_t color_d_mapping : 3;
        /* 4 * 8 bits */
        input_t   alpha_a_input : 5;
        mapping_t alpha_a_mapping : 3;
        input_t   alpha_b_input : 5;
        mapping_t alpha_b_mapping : 3;
        input_t   alpha_c_input : 5;
        mapping_t alpha_c_mapping : 3;
        input_t   alpha_d_input : 5;
        mapping_t alpha_d_mapping : 3;
        /* 17 bits */
        output_t      color_ab_output : 4;
        output_func_t color_ab_out_func : 1;
        output_t      color_cd_output : 4;
        output_func_t color_cd_out_func : 1;
        output_t      color_ab_cd_mux_sum : 4;
        blam::shader::shader_transparent::output_mapping_t color_output_map : 3;
        /* 15 bits */
        output_t alpha_ab_output : 4;
        output_t alpha_cd_output : 4;
        output_t alpha_ab_cd_mux_sum : 4;
        blam::shader::shader_transparent::output_mapping_t alpha_output_map : 3;
    };

    stage_t stages[5];
    u32     padding;

    STATICINLINE stage_t
    from_blam([[maybe_unused]] blam::shader::shader_transparent::stage_t const& stage)
    {

        return {
//            .color_a_input   = static_cast<input_t>(stage.color.a_input),
//            .color_a_mapping = stage.color.a_mapping,
//            .color_b_input   = static_cast<input_t>(stage.color.b_input),
//            .color_b_mapping = stage.color.b_mapping,
//            .color_c_input   = static_cast<input_t>(stage.color.c_input),
//            .color_c_mapping = stage.color.c_mapping,
//            .color_d_input   = static_cast<input_t>(stage.color.d_input),
//            .color_d_mapping = stage.color.d_mapping,

//            .alpha_a_input   = static_cast<input_t>(stage.alpha.a_input),
//            .alpha_a_mapping = stage.alpha.a_mapping,
//            .alpha_b_input   = static_cast<input_t>(stage.alpha.b_input),
//            .alpha_b_mapping = stage.alpha.b_mapping,
//            .alpha_c_input   = static_cast<input_t>(stage.alpha.c_input),
//            .alpha_c_mapping = stage.alpha.c_mapping,
//            .alpha_d_input   = static_cast<input_t>(stage.alpha.d_input),
//            .alpha_d_mapping = stage.alpha.d_mapping,

//            .color_ab_output     = stage.color.ab_output,
//            .color_ab_out_func   = stage.color.ab_out_func,
//            .color_cd_output     = stage.color.cd_output,
//            .color_cd_out_func   = stage.color.cd_out_func,
//            .color_ab_cd_mux_sum = stage.color.ab_cd_mux_sum,
//            .color_output_map    = stage.color.output_map,

//            .alpha_ab_output     = stage.alpha.ab_output,
//            .alpha_cd_output     = stage.alpha.cd_output,
//            .alpha_ab_cd_mux_sum = stage.alpha.ab_cd_mux_sum,
//            .alpha_output_map    = stage.alpha.output_map,
        };
    }
};

/* TODO: Fix this on MinGW */
// static_assert(sizeof(transparent_data::stage_t) == 3 * sizeof(u32));
// static_assert(sizeof(transparent_data) == 64);

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
    fog_properties   fog;
};

static_assert(sizeof(world_data) == 144);

} // namespace materials
