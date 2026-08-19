#pragma once

#include <peripherals/libc/types.h>
#include <peripherals/stl/range.h>
#include <peripherals/typing/vectors/vector_types.h>

#include <blam/volta/blam_shaders.h>

#include <stdexcept>

namespace materials {

using libc_types::f32;
using libc_types::i32;
using libc_types::u32;
using typing::vector_types::Vecf2;
using typing::vector_types::Vecf4;

constexpr u32 flag_interior = 0x10000;

template<typename... Values>
Vecf4 partitioned_vec4(Values... values)
{
    size_t                               num_values = sizeof...(Values);
    std::array<Vecf4, sizeof...(Values)> vecs       = {{values...}};
    Vecf4                                out{};
    f32                                  offset = 1.f / num_values;
    for(auto i : stl_types::range<size_t>(num_values))
    {
        out += (vecs[i] / num_values) + Vecf4(offset * i);
    }
    return out;
}

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

/* Per-instance stage data for shader_transparent (sotr).
 * Packed as GPU-native u32 fields so it maps directly to the GLSL SSBO.
 * Layout mirrors TransparentData in map_sampling.glsl. */
struct alignas(16) transparent_data
{
    struct alignas(16) stage_t
    {
        /* color_in:  4 × (5-bit input + 3-bit mapping) = 32 bits
         *            uses shader_transparent::input_t enum */
        u32 color_in;
        /* alpha_in:  same packing using blam::shader::color_input enum */
        u32 alpha_in;
        /* outputs:   color_out bits [0..16] | alpha_out bits [17..31]
         *   color: ab_dst[4] ab_fn[1] cd_dst[4] cd_fn[1] sum_dst[4] omap[3]
         *          = 17 bits, so alpha MUST start at bit 17 — packing it at
         *          16 bleeds alpha ab_dst's LSB into the color output map
         *   alpha: ab_dst[4] cd_dst[4] sum_dst[4] omap[3] = 15 bits */
        u32   outputs;
        u32   flags;     /* stage_flags_t */
        Vecf4 color0;    /* constant_color0: animated tint (time-animated on
                            CPU; = lower bound when a_out controls the anim) */
        Vecf4 color0_up; /* constant_color0 upper bound, for the
                            a_out_controls_color0_anim flag (0x4): the shader
                            mixes color0..color0_up by the scratch alpha */
        Vecf4 color1;    /* constant_color1: static tint */

        STATICINLINE stage_t
        from_blam(blam::shader::shader_transparent::stage_t const& s)
        {
            auto pack_inputs = [](auto ai,
                                  auto am,
                                  auto bi,
                                  auto bm,
                                  auto ci,
                                  auto cm,
                                  auto di,
                                  auto dm) -> u32 {
                return (static_cast<u32>(ai) & 0x1Fu) << 0 |
                       (static_cast<u32>(am) & 0x07u) << 5 |
                       (static_cast<u32>(bi) & 0x1Fu) << 8 |
                       (static_cast<u32>(bm) & 0x07u) << 13 |
                       (static_cast<u32>(ci) & 0x1Fu) << 16 |
                       (static_cast<u32>(cm) & 0x07u) << 21 |
                       (static_cast<u32>(di) & 0x1Fu) << 24 |
                       (static_cast<u32>(dm) & 0x07u) << 29;
            };

            u32 color_in = pack_inputs(
                s.color.a_input,
                s.color.a_mapping,
                s.color.b_input,
                s.color.b_mapping,
                s.color.c_input,
                s.color.c_mapping,
                s.color.d_input,
                s.color.d_mapping);

            u32 alpha_in = pack_inputs(
                s.alpha.a_input,
                s.alpha.a_mapping,
                s.alpha.b_input,
                s.alpha.b_mapping,
                s.alpha.c_input,
                s.alpha.c_mapping,
                s.alpha.d_input,
                s.alpha.d_mapping);

            u32 color_out =
                (static_cast<u32>(s.color.ab_output) & 0xFu) << 0 |
                (static_cast<u32>(s.color.ab_out_func) & 0x1u) << 4 |
                (static_cast<u32>(s.color.cd_output) & 0xFu) << 5 |
                (static_cast<u32>(s.color.cd_out_func) & 0x1u) << 9 |
                (static_cast<u32>(s.color.ab_cd_mux_sum) & 0xFu) << 10 |
                (static_cast<u32>(s.color.output_map) & 0x7u) << 14;

            u32 alpha_out = (static_cast<u32>(s.alpha.ab_output) & 0xFu) << 0 |
                            (static_cast<u32>(s.alpha.cd_output) & 0xFu) << 4 |
                            (static_cast<u32>(s.alpha.ab_cd_mux_sum) & 0xFu)
                                << 8 |
                            (static_cast<u32>(s.alpha.output_map) & 0x7u) << 12;

            /* Tag colors are ARGB floats (alpha first) — reorder to RGBA.
             * Verified against 'light dim blue' c1=(0,.55,.60,.84) → blue. */
            auto argb = [](Vecf4 const& c) -> Vecf4 {
                return Vecf4(c.y, c.z, c.w, c.x);
            };
            /* A constant color of all-zero is "unset"; as a combiner
             * multiplier that would nuke the stage to black. Treat it as
             * white (identity) so e.g. holo curtains keep their texture. */
            auto ident = [&argb](Vecf4 c) -> Vecf4 {
                return (c.x == 0.f && c.y == 0.f && c.z == 0.f && c.w == 0.f)
                           ? Vecf4(1.f)
                           : argb(c);
            };
            bool a_out_anim =
                (static_cast<u32>(s.flags) & 0x4u) != 0; /* per-pixel anim */
            bool unset =
                s.color0_lower == Vecf4(0) && s.color0_upper == Vecf4(0);
            return {
                .color_in  = color_in,
                .alpha_in  = alpha_in,
                .outputs   = color_out | (alpha_out << 17),
                .flags     = static_cast<u32>(s.flags),
                .color0    = unset ? Vecf4(1)
                             : a_out_anim
                                 ? argb(s.color0_lower)
                                 : argb((s.color0_lower + s.color0_upper) * 0.5f),
                .color0_up = unset ? Vecf4(1) : argb(s.color0_upper),
                .color1    = ident(s.color1),
            };
        }
    };

    u32 num_stages;
    u32 blend_mode; /* chicago::framebuffer_blending */
    u32 padding[2];
    /* 7 stages: the NV2A runs up to 8 combiner stages and tags use them
     * (generator shield = 7); truncating to 4 cut off the final compose. */
    stage_t stages[7];
};

static_assert(sizeof(transparent_data::stage_t) == 64);
static_assert(sizeof(transparent_data) == 464);

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

constexpr size_t max_world_lights = 8;

struct alignas(16) world_data
{
    light_properties lighting[max_world_lights];
    fog_properties   fog;
};

static_assert(sizeof(world_data) == 336);

} // namespace materials
