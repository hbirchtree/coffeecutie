#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "fragments/map_fragment_input.glsl"
#include "fragments/map_sampling.glsl"

layout(location = 10, binding = 0) uniform sampler2DArray source_bc1;
layout(location = 11, binding = 1) uniform sampler2DArray source_bc2;
layout(location = 12, binding = 2) uniform sampler2DArray source_bc3;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgb565;
// lightmap
layout(location = 15, binding = 5) uniform sampler2DArray source_r8;
layout(location = 16, binding = 6) uniform sampler2DArray source_rg8;
layout(location = 17, binding = 7) uniform sampler2DArray source_rgba4;
layout(location = 18, binding = 8) uniform sampler2DArray source_rgba8;

vec4 sample_map(in uint map_id, sampler2DArray sampler)
{
    return get_map(map_id, sampler, frag.tex, frag.instanceId);
}

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGB565 = 4u;
const uint TEX_R8     = 5u;
const uint TEX_RG8    = 6u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;

vec4 get_color(in uint map_id)
{
    int tex_id = mats.instance[frag.instanceId].maps[map_id].layer;
    uint source = tex_id >> 24;
    if(source == TEX_BC1)
        return sample_map(map_id, source_bc1);
    else if(source == TEX_BC2)
        return sample_map(map_id, source_bc2);
    else if(source == TEX_BC3)
        return sample_map(map_id, source_bc3);
    else if(source == TEX_R8)
        return sample_map(map_id, source_r8);
    else if(source == TEX_RG8)
        return sample_map(map_id, source_rg8);
    else if(source == TEX_RGB565)
        return sample_map(map_id, source_rgb565);
    else if(source == TEX_RGBA4)
        return sample_map(map_id, source_rgba4);
    else if(source == TEX_RGBA8)
        return sample_map(map_id, source_rgba8);
    return vec4(vec3(1.0, 1.0, 1.0), 1.0);
}

layout(location = 0) out vec4 final_color;

const uint base_map_id      = 0u;
const uint micro_map_id     = 1u;
const uint primary_map_id   = 2u;
const uint secondary_map_id = 3u;

const uint MATERIAL_SENV = 1u;
const uint MATERIAL_SWAT = 2u;
const uint MATERIAL_SGLA = 3u;

void main()
{
    uint material_id = get_material_id(frag.instanceId);

    if(material_id == MATERIAL_SENV)
    {
        vec4 base = get_color(base_map_id);
        vec4 micro = get_color(micro_map_id);
        vec4 primary = get_color(primary_map_id);
        vec4 secondary = get_color(secondary_map_id);
        vec4 lightmap = get_light(frag.instanceId, frag.light_tex);

        final_color = vec4(
            base.rgb *
            micro.rgb *
            ((primary.rgb * base.a) + (secondary.rgb * (1.0 - base.a))) *
            lightmap.rgb, 1.0);
    } else if(material_id == MATERIAL_SWAT)
    {
        vec4 base = get_color(base_map_id);
        final_color = vec4(base.rgb, 0.2);
    } else if(material_id == MATERIAL_SGLA)
    {
        vec4 base = get_color(base_map_id);
        final_color = base;
    } else
        final_color = vec4(0.0, 1.0, 0.0, 1.0);
}
