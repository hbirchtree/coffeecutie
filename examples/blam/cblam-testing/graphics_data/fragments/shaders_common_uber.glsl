#include "map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 world_pos;
    vec2 tex;
    vec3 normal;
    vec3 binormal;
    vec3 tangent;
#if USE_BSP_SHADERS == 1
    vec2 light_tex;
#endif
    flat int instanceId;
} frag;

layout(location = 10, binding = 0) uniform sampler2DArray source_bc1;
layout(location = 11, binding = 1) uniform sampler2DArray source_bc2;
layout(location = 12, binding = 2) uniform sampler2DArray source_bc3;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgb565;
// lightmap
layout(location = 15, binding = 5) uniform sampler2DArray source_r8;
layout(location = 16, binding = 6) uniform sampler2DArray source_rg8;
layout(location = 17, binding = 7) uniform sampler2DArray source_rgba4;
layout(location = 18, binding = 8) uniform sampler2DArray source_rgba8;

layout(location = 20) uniform vec3 camera_position;

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
    {
        vec4 color = sample_map(map_id, source_bc1);
//        vec3 background_color = texture(source_bc1, vec3(0, 0, tex_id)).rgb;
//        color.a = length(background_color - color.rgb) < 0.1 ? 0.0 : 1.0;
        return color;
    } else if(source == TEX_BC2)
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
const uint MATERIAL_SCHI = 4u;
const uint MATERIAL_SCEX = 5u;
const uint MATERIAL_SMET = 6u;
const uint MATERIAL_SOTR = 7u;
const uint MATERIAL_SPLA = 8u;
const uint MATERIAL_SOSO = 9u;

void main()
{
    uint material_id = get_material_id(frag.instanceId);

    vec3 camera_distance = frag.world_pos * camera_position;

    if(material_id == MATERIAL_SENV)
    {
        const uint TYPE_NORMAL           = 1u;
        const uint TYPE_BLENDED          = 2u;
        const uint TYPE_BLENDED_SPECULAR = 3u;

        const uint type = TYPE_BLENDED_SPECULAR;

        vec4 base = get_color(base_map_id);
        vec4 micro = get_color(micro_map_id);
        vec4 primary = get_color(primary_map_id);
        vec4 secondary = get_color(secondary_map_id);
#if USE_BSP_SHADERS == 1
        vec4 lightmap = get_light(frag.instanceId, frag.light_tex);
#endif

        float factor = type == TYPE_NORMAL ? secondary.a : base.a;
        vec4 blend = ((primary * factor) + (secondary * (1.0 - factor)));
        float specular = type == TYPE_BLENDED_SPECULAR
            ? base.a * micro.a : blend.a * micro.a;

        float camera_angle = dot(camera_distance, frag.normal);
        specular = specular * 1.0 + 0.0001 * camera_angle;
        specular = 1.0;

        final_color = vec4(
            base.rgb *
            micro.rgb *
            blend.rgb *
#if USE_BSP_SHADERS == 1
            lightmap.rgb *
#endif
            vec3(1.0) * (specular * 0.01 + 0.99),
//            base.a
            1.0
            );
    } else if(material_id == MATERIAL_SWAT)
    {
        vec4 base = get_color(base_map_id);
        final_color = vec4(0, 0, 1, base.r);
//        final_color = vec4(base.rgb, 0.1);
    } else if(material_id == MATERIAL_SGLA)
    {
        vec4 base = get_color(base_map_id);
//        final_color = vec4(1, 0, 0, 0.3);
        final_color = vec4(base.rgba);
    } else if(material_id == MATERIAL_SMET)
    {
        final_color = vec4(vec3(0.3), 1);
    } else if(material_id == MATERIAL_SCHI)
    {
        vec4 base = get_color(base_map_id);
        final_color = vec4(base.rgba);
    } else if(material_id == MATERIAL_SCEX)
    {
        vec4 base = get_color(base_map_id);
        final_color = base;
//        final_color = vec4(1, 1, 0, 1);
#if USE_MODEL_SHADERS == 1
    } else if(material_id == MATERIAL_SOSO)
    {
        vec4 base = get_color(base_map_id);
        if(base.a < 0.5)
            discard;
        final_color = vec4(base.rgba);
#endif
    } else
        final_color = vec4(0, 1, 0, 1);
}
