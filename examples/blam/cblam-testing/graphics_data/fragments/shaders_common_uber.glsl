#include "map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 tbn_direction;
    vec3 eye_direction;
    vec3 position;
    vec2 tex;
    mat3 tbn;
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

#if USE_REFLECTIONS == 1 && USE_ARRAY_CUBEMAP == 1
layout(location = 19, binding = 9) uniform samplerCubeArray source_cube_bc1;
layout(location = 20, binding = 10) uniform samplerCubeArray source_cube_rgb565;
layout(location = 30, binding = 11) uniform samplerCubeArray source_cube_rgba8;
#elif USE_REFLECTIONS == 1
layout(location = 19, binding = 9) uniform samplerCube source_cube_bc1;
layout(location = 20, binding = 10) uniform samplerCube source_cube_rgb565;
layout(location = 30, binding = 11) uniform samplerCube source_cube_rgba8;
#endif

layout(location = 21) uniform vec3 camera_position;

struct LightProperties
{
    vec4 light_direction;
    vec4 light_color;
};

layout(binding = 2, std140) uniform WorldProperties
{
    LightProperties lighting[2];
} world;

const uint INTERIOR_LIGHTING = 0u;
const uint EXTERIOR_LIGHTING = 1u;

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
        return sample_map(map_id, source_rgb565).bgra;
    else if(source == TEX_RGBA4)
        return sample_map(map_id, source_rgba4).bgra;
    else if(source == TEX_RGBA8)
        return sample_map(map_id, source_rgba8).bgra;
    return vec4(vec3(1.0, 1.0, 1.0), 1.0);
}

#if USE_REFLECTIONS == 1
vec4 get_cube_color(in vec3 tex_coord)
{
    int tex_id = mats.instance[frag.instanceId].lightmap.reflection;
    uint source = tex_id >> 24;
    if(source == TEX_BC1)
        return get_cube_map(source_cube_bc1, tex_coord, frag.instanceId);
    else if(source == TEX_RGB565)
        return get_cube_map(source_cube_rgb565, tex_coord, frag.instanceId);
    else if(source == TEX_RGBA8)
        return get_cube_map(source_cube_rgba8, tex_coord, frag.instanceId);
    else
        return vec4(vec3(1, 0, 1), 1.0);
}
#endif

const uint base_map_id      = 0u;

vec3 view_direction()
{
    return normalize(frag.tbn * (camera_position - frag.position));
}
vec3 light_direction()
{
    vec3 direction = world.lighting[INTERIOR_LIGHTING].light_direction.xyz;
    return normalize(
        transpose(frag.tbn) *
        direction * -1);
}

vec4 shader_dummy()
{
    return get_color(base_map_id);
}

vec4 shader_environment()
{
    const int TYPE_NORMAL           = 0;
    const int TYPE_BLENDED          = 1;
    const int TYPE_BLENDED_SPECULAR = 2;

    const uint micro_map_id     = 1u;
    const uint primary_map_id   = 2u;
    const uint secondary_map_id = 3u;
    const uint bump_map_id      = 4u;

    int flags = mats.instance[frag.instanceId].material.flags & 0x7;
    int type = (mats.instance[frag.instanceId].material.flags >> 4) & 0x3;

    vec4 base = get_color(base_map_id);
    vec4 micro = get_color(micro_map_id);
    vec4 primary = get_color(primary_map_id);
    vec4 secondary = get_color(secondary_map_id);
#if USE_LIGHTMAPS == 1
    vec4 lightmap = get_light(frag.instanceId, frag.light_tex);
#endif

    float factor = type == TYPE_NORMAL ? secondary.a : base.a;
    vec4 blend = ((primary * factor) + (secondary * (1.0 - factor)));
    float specular = type == TYPE_BLENDED_SPECULAR
        ? base.a * micro.a : blend.a * micro.a;

#if USE_NORMALMAP == 1
    vec3 normal = normalize(get_color(bump_map_id).rgb * 2.0 - 1.0);
    normal = frag.tbn * normal;
    float normal_factor = dot(normal, -light_direction());
#endif

#if USE_REFLECTIONS == 1
    int reflective = mats.instance[frag.instanceId].material.flags & 0x40;
    vec3 reflection = vec3(1.0);
    vec3 reflection_tint = vec3(1.0);
    if(reflective > 0)
    {
        int reflect_flags = (mats.instance[frag.instanceId].material.flags >> 6) & 0x3;
        reflection = get_cube_color(view_direction() * vec3(1, -1, 1)).rgb;

        vec4 perp_color = mats.instance[frag.instanceId].material.input2;
        vec4 parallel_color = mats.instance[frag.instanceId].material.input3;
        reflection_tint = mix(
            perp_color.rgb * perp_color.a,
            parallel_color.rgb * parallel_color.a,
            dot(normal, view_direction()));
    }
#endif

    return vec4(
        base.rgb *
        micro.rgb *
        blend.rgb *
#if USE_LIGHTMAPS == 1
        lightmap.rgb *
#endif
#if USE_REFLECTIONS == 1
        mix(reflection, vec3(1), base.a) *
//        reflection_tint *
#endif
#if USE_NORMALMAP == 1
        max(0.1, normal_factor) *
        specular *
#endif
        vec3(1),
        1.0
        );
}

vec4 shader_chicago()
{
//    return shader_dummy();
    return vec4(1, 0, 1, 1);
}

vec4 shader_chicago_extended()
{
//    return shader_dummy();
    return vec4(1, 0, 1, 1);
}

vec4 shader_transparent()
{
    return vec4(shader_dummy().rgb, 1.0);
}

vec4 shader_model()
{
    const uint multi_map_id  = 1u;
    const uint detail_map_id = 2u;

    vec4 color = get_color(base_map_id);
    vec4 multi = get_color(multi_map_id);
    vec4 detail = get_color(detail_map_id);
#if USE_REFLECTIONS == 1
    vec3 reflection = get_cube_color(view_direction()).rgb;
    float reflection_factor = 0;
    if((mats.instance[frag.instanceId].material.flags & 0x1) != 0)
        reflection_factor = multi.b;
    reflection_factor = 1;
#endif

    if(color.a < 0.5)
        discard;
    return vec4(
        color.rgb *
        detail.rgb *
#if USE_REFLECTIONS == 1
        mix(vec3(1), reflection, reflection_factor) *
#endif
        1, 1);
}

vec4 shader_glass()
{
    return shader_dummy();
}

vec4 shader_metal()
{
    return vec4(shader_dummy().rgb, 1.0);
}

vec4 shader_plasma()
{
    return vec4(shader_dummy().rgba);
}

vec4 shader_water()
{
    const int ALPHA_MODULATES_REFLECT    = 0x1;
    const int COLOR_MODULATES_BACKGROUND = 0x2;

    const uint reflection_map_id = 1u;

    int flags = mats.instance[frag.instanceId].material.flags & 0x3;

    vec4 base = get_color(base_map_id);
#if USE_REFLECTIONS == 1
    vec4 reflection = get_cube_color(view_direction());

    if((flags & ALPHA_MODULATES_REFLECT) != 0)
        reflection.rgb = reflection.rgb * base.a;
    if((flags & COLOR_MODULATES_BACKGROUND) != 0)
        reflection.rgb = reflection.rgb * base.rgb;

    return vec4(reflection.rgb, base.r);
#else
    return vec4(0, 0, 1, base.r);
#endif
}

const uint MATERIAL_SENV = 1u;
const uint MATERIAL_SWAT = 2u;
const uint MATERIAL_SGLA = 3u;
const uint MATERIAL_SCHI = 4u;
const uint MATERIAL_SCEX = 5u;
const uint MATERIAL_SMET = 6u;
const uint MATERIAL_SOTR = 7u;
const uint MATERIAL_SPLA = 8u;
const uint MATERIAL_SOSO = 9u;

layout(location = 0) out vec4 final_color;

void main()
{
    uint material_id = get_material_id(frag.instanceId);

    if(material_id == MATERIAL_SENV)
    {
        final_color = shader_environment();
#if USE_MODEL_SHADERS == 1
    } else if(material_id == MATERIAL_SOSO)
    {
        final_color = shader_model();
#endif
    } else if(material_id == MATERIAL_SCHI)
    {
        final_color = shader_chicago();
    } else if(material_id == MATERIAL_SCEX)
    {
        final_color = shader_chicago_extended();
    } else if(material_id == MATERIAL_SWAT)
    {
        final_color = shader_water();
    } else if(material_id == MATERIAL_SGLA)
    {
        final_color = shader_glass();
    } else if(material_id == MATERIAL_SMET)
    {
        final_color = vec4(vec3(0.3), 1);
    } else if(material_id == MATERIAL_SOTR)
    {
        final_color = shader_transparent();
    } else
        final_color = vec4(0, 1, 0, 1);
}
