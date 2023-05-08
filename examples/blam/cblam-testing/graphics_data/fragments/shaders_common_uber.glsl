#include "map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 tbn_direction;
    vec3 eye_direction;
    vec3 position;
    vec2 tex;
    mat3 tbn;
#if USE_LIGHTMAPS == 1
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
layout(location = 22) uniform float time;

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

vec4 sample_map(
    in uint map_id,
    in int layer,
    in sampler2DArray sampler,
    in vec2 offset)
{
    return get_map(
        map_id,
        layer,
        sampler,
        frag.tex + offset,
        frag.instanceId);
}

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGB565 = 4u;
const uint TEX_R8     = 5u;
const uint TEX_RG8    = 6u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;

vec4 get_color_explicit_with_offset(in uint map_id, in int tex_id, in vec2 offset)
{
    uint source = tex_id >> 24;
    if(source == TEX_BC1)
        return sample_map(map_id, tex_id, source_bc1, offset);
    else if(source == TEX_BC2)
        return sample_map(map_id, tex_id, source_bc2, offset);
    else if(source == TEX_BC3)
        return sample_map(map_id, tex_id, source_bc3, offset);
    else if(source == TEX_R8)
        return sample_map(map_id, tex_id, source_r8, offset);
    else if(source == TEX_RG8)
        return sample_map(map_id, tex_id, source_rg8, offset);
    else if(source == TEX_RGB565)
        return sample_map(map_id, tex_id, source_rgb565, offset).bgra;
    else if(source == TEX_RGBA4)
        return sample_map(map_id, tex_id, source_rgba4, offset).bgra;
    else if(source == TEX_RGBA8)
        return sample_map(map_id, tex_id, source_rgba8, offset).bgra;
    return vec4(vec3(1), 1);
}

vec4 get_color_with_offset(in uint map_id, in vec2 offset)
{
    int tex_id = mats.instance[frag.instanceId].maps[map_id].layer;
    return get_color_explicit_with_offset(map_id, tex_id, offset);
}

vec4 get_color(in uint map_id)
{
    return get_color_with_offset(map_id, vec2(0));
}

vec4 get_color_explicit(in uint map_id, in int layer)
{
    return get_color_with_offset(map_id, vec2(0));
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
        return vec4(1);
}
#endif

#if USE_NORMALMAP == 1
vec4 get_bump(in uint map_id, in vec2 offset)
{
    vec3 normal = normalize(get_color_with_offset(map_id, offset).rgb * 2.0 - 1.0);
    normal = frag.tbn * normal;
    return vec4(normal, dot(normal, -light_direction()));
}
#endif

const uint base_map_id      = 0u;

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

    uint flags = uint(mats.instance[frag.instanceId].material.flags & 0x7);
    uint type = uint((mats.instance[frag.instanceId].material.flags >> 4) & 0x3);

    int detailed = (mats.instance[frag.instanceId].material.flags >> 9) & 0x1;
    int has_micro = (mats.instance[frag.instanceId].material.flags >> 10) & 0x1;
    detailed = 1;
    has_micro = 1;

    vec4 base = get_color(base_map_id);
    vec4 micro = has_micro == 1 ? get_color(micro_map_id) : vec4(1);
    vec4 primary = get_color(primary_map_id);
    vec4 secondary = get_color(secondary_map_id);
#if USE_LIGHTMAPS == 1
    vec4 lightmap = get_light(frag.instanceId, frag.light_tex);
#endif

    float factor = type == TYPE_NORMAL ? secondary.a : base.a;
    vec4 blend = detailed == 1
        ? ((primary * factor) + (secondary * (1.0 - factor)))
        : vec4(1);
    float specular = type == TYPE_BLENDED_SPECULAR
        ? base.a * micro.a : blend.a * micro.a;

#if USE_NORMALMAP == 1
    vec4 normal = get_bump(bump_map_id, vec2(0));
#endif

#if USE_REFLECTIONS == 1
    int reflective = (mats.instance[frag.instanceId].material.flags >> 6) & 0x1;
    vec3 reflection = vec3(1.0);
    vec3 reflection_tint = vec3(1.0);
    if(reflective > 0)
    {
        uint reflect_flags = uint((mats.instance[frag.instanceId].material.flags >> 7) & 0x3);
        reflection = get_cube_color(view_direction() * vec3(1, -1, 1)).rgb;

        vec4 perp_color = mats.instance[frag.instanceId].material.input2;
        vec4 parallel_color = mats.instance[frag.instanceId].material.input3;
        reflection_tint = mix(
            perp_color.rgb * perp_color.a,
            parallel_color.rgb * parallel_color.a,
            dot(normal.rgb, -view_direction()));
    }
#endif
#if USE_SELF_ILLUMINATION == 1
    int self_illum_layer = mats.instance[frag.instanceId].lightmap.meta1;
    vec4 self_illum = get_color_explicit(base_map_id, self_illum_layer);

    // get power on/off colors for prim/secondary/plasma
    // that's 3 * 2 * 3 values
#endif

//    if(type == TYPE_NORMAL)
//        return vec4(vec3(base.a * micro.a), 1);
//    else if(type == TYPE_BLENDED)
//        return vec4(vec3(secondary.a), 1);
//    else
//        return vec4(1, 0, 0, 1);

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
//        1 +
#endif
#if USE_NORMALMAP == 1
        max(0.1, normal.a) *
#endif
        vec3(1),
        1.0
        );
}

#if USE_CHICAGO == 1
#define CHICAGO_DEBUG 1

void combine_map(
    inout vec4 out_color,
    in vec4 color,
    in vec4 next_color,
    in uint flags)
{
    const uint F_CURRENT            = 0;
    const uint F_NEXT               = 1;
    const uint F_MUL                = 2;
    const uint F_DOUBLE_MUL         = 3;
    const uint F_ADD                = 4;
    const uint F_ADD_SIGNED_CURRENT = 5;
    const uint F_ADD_SIGNED_NEXT    = 6;
    const uint F_SUB_SIGNED_CURRENT = 7;
    const uint F_SUB_SIGNED_NEXT    = 8;

    const uint F_BLEND_CURRENT_ALPHA         = 9;
    const uint F_BLEND_CURRENT_ALPHA_INVERSE = 10;
    const uint F_BLEND_NEXT_ALPHA            = 11;
    const uint F_BLEND_NEXT_ALPHA_INVERSE    = 12;

    uint color_func = flags & 0x1F;
    uint alpha_func = flags >> 5;

    if(color_func == F_CURRENT)
        ;
    else if(color_func == F_NEXT)
        out_color.rgb = next_color.rgb;
    else if(color_func == F_MUL)
        out_color.rgb = out_color.rgb * color.rgb;
    else if(color_func == F_DOUBLE_MUL)
        out_color.rgb = out_color.rgb * color.rgb * color.rgb;
    else if(color_func == F_ADD || color_func == F_ADD_SIGNED_CURRENT)
        out_color.rgb = out_color.rgb + color.rgb;
    else if(color_func == F_ADD_SIGNED_NEXT)
        out_color.rgb = out_color.rgb + next_color.rgb;
    else if(color_func == F_SUB_SIGNED_CURRENT)
        out_color.rgb = out_color.rgb - color.rgb;
    else if(color_func == F_SUB_SIGNED_NEXT)
        out_color.rgb = out_color.rgb - next_color.rgb;
    else if(color_func == F_BLEND_CURRENT_ALPHA)
        out_color.rgb = out_color.rgb + color.rgb * color.a;
    else if(color_func == F_BLEND_CURRENT_ALPHA_INVERSE)
        out_color.rgb = out_color.rgb + color.rgb * (1 - color.a);
    else if(color_func == F_BLEND_NEXT_ALPHA)
        out_color.rgb = out_color.rgb + color.rgb * next_color.a;
    else if(color_func == F_BLEND_NEXT_ALPHA_INVERSE)
        out_color.rgb = out_color.rgb + color.rgb * (1 - next_color.a);
    else
        out_color.rgb = vec3(1, 0, 1);

    if(alpha_func == F_CURRENT)
        ;
    if(alpha_func == F_ADD)
        out_color.a = out_color.a + color.a;
    else if(alpha_func == F_MUL)
        out_color.a = out_color.a + color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA)
        out_color.a = out_color.a + color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA_INVERSE)
        out_color.a = out_color.a + (1 - color.a);
    else if(alpha_func == F_BLEND_NEXT_ALPHA)
        out_color.a = out_color.a + next_color.a;
    else if(alpha_func == F_BLEND_NEXT_ALPHA_INVERSE)
        out_color.a = out_color.a + (1 - next_color.a);
    else
        out_color.rgb = vec3(1, 1, 0);
}

const uint F_CURRENT            = 0;
const uint F_NEXT               = 1;
const uint F_MUL                = 2;
const uint F_DOUBLE_MUL         = 3;
const uint F_ADD                = 4;
const uint F_ADD_SIGNED_CURRENT = 5;
const uint F_ADD_SIGNED_NEXT    = 6;
const uint F_SUB_SIGNED_CURRENT = 7;
const uint F_SUB_SIGNED_NEXT    = 8;

const uint F_BLEND_CURRENT_ALPHA         = 9;
const uint F_BLEND_CURRENT_ALPHA_INVERSE = 10;
const uint F_BLEND_NEXT_ALPHA            = 11;
const uint F_BLEND_NEXT_ALPHA_INVERSE    = 12;

void init_map_v2(
    inout vec4 dst,
    in vec4 color,
    in vec4 next_color,
    in uint flags)
{
    uint color_func = flags & 0xF;
    uint alpha_func = flags >> 4;

    if(color_func == F_CURRENT)
        dst.rgb = color.rgb;
    else if(color_func == F_NEXT)
        dst.rgb = next_color.rgb;
    else if(color_func == F_MUL)
        dst.rgb = color.rgb * next_color.rgb;
    else if(color_func == F_DOUBLE_MUL)
        dst.rgb = color.rgb * next_color.rgb * next_color.rgb;
    else if(color_func == F_ADD)
        dst.rgb = color.rgb + next_color.rgb;
#if CHICAGO_DEBUG == 1
    else
        dst.rgb = vec3(1, 0, 1);
#endif

    if(alpha_func == F_CURRENT)
        dst.a = color.a;
    if(alpha_func == F_MUL)
        dst.a = color.a * next_color.a;
    if(alpha_func == F_ADD)
        dst.a = color.a + next_color.a;
    if(alpha_func == F_DOUBLE_MUL)
        dst.a = color.a * next_color.a * next_color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA)
        dst.a = color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA_INVERSE)
//        dst.a = (1 - color.a) + next_color.a;
        dst.a = next_color.a * (1 - color.a) + next_color.a;
    else if(alpha_func == F_BLEND_NEXT_ALPHA)
        dst.a = (color.a * (1 - next_color.a)) + next_color.a;
    else if(alpha_func == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.a = (1 - next_color.a);
}

void combine_map_v2(
    inout vec4 dst,
    in vec4 color,
    in vec4 next_color,
    in uint flags)
{
    uint color_func = flags & 0xF;

    vec3 src = vec3(0);

    if(color_func == F_CURRENT)
        src.rgb = color.rgb;
    else if(color_func == F_NEXT)
        src.rgb = next_color.rgb;
    else if(color_func == F_MUL)
        src.rgb = color.rgb;
    else if(color_func == F_DOUBLE_MUL)
        src.rgb = color.rgb;
    else if(color_func == F_ADD)
        src.rgb = color.rgb;
    else if(color_func == F_ADD_SIGNED_CURRENT)
        src.rgb = color.rgb;
    else if(color_func == F_ADD_SIGNED_NEXT)
        src.rgb = next_color.rgb;
    else if(color_func == F_SUB_SIGNED_CURRENT)
        src.rgb = color.rgb;
    else if(color_func == F_SUB_SIGNED_NEXT)
        src.rgb = next_color.rgb;
#if CHICAGO_DEBUG == 1
//    else
//        src.rgb = vec3(1, 0, 1);
#endif

    uint alpha_func = flags >> 4;

    if(color_func == F_CURRENT)
        ;
    else if(color_func == F_MUL)
        dst.rgb = dst.rgb * src.rgb;
    else if(color_func == F_DOUBLE_MUL)
        dst.rgb = dst.rgb * src.rgb * src.rgb;
    else if(color_func == F_ADD ||
            color_func == F_ADD_SIGNED_CURRENT ||
            color_func == F_ADD_SIGNED_NEXT)
        dst.rgb = dst.rgb + src.rgb;
    else if(color_func == F_SUB_SIGNED_CURRENT ||
            color_func == F_SUB_SIGNED_NEXT)
        dst.rgb = dst.rgb - src.rgb;
    else if(color_func == F_BLEND_CURRENT_ALPHA)
        dst.rgb = dst.rgb * (1 - color.a) + color.rgb * color.a;
//    else if(color_func == F_BLEND_CURRENT_ALPHA_INVERSE)
//        dst.rgb = dst.rgb * color.a + src.rgb * (1 - color.a);
//    else if(color_func == F_BLEND_NEXT_ALPHA)
//        dst.rgb = dst.rgb * (1 - color.a) + src.rgb * color.a;
//    else if(color_func == F_BLEND_NEXT_ALPHA_INVERSE)
//        dst.rgb = dst.rgb * next_color.a + src.rgb * (1 - next_color.a);
#if CHICAGO_DEBUG == 1
//    else
//        dst = vec4(1, 0, 1, 1);
#endif


    if(alpha_func == F_CURRENT)
        ;
    else if(alpha_func == F_MUL)
        dst.a = dst.a * color.a;
    else if(alpha_func == F_ADD)
        dst.a = dst.a + color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA)
        dst.a = dst.a + color.a;
    else if(alpha_func == F_BLEND_CURRENT_ALPHA_INVERSE)
        dst.a = dst.a + (1 - color.a);
    else if(alpha_func == F_BLEND_NEXT_ALPHA)
        dst.a = dst.a * (1 - next_color.a) + next_color.a;
    else if(alpha_func == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.a = dst.a * next_color.a + (1 - next_color.a);
#if CHICAGO_DEBUG == 1
    else
        dst = vec4(1, 0, 1, 1);
#endif
}

vec4 shader_chicago()
{
    vec4 out_color = vec4(vec3(0), 0);

    vec2 o1 = mats.instance[frag.instanceId].material.input1.xy;
    vec2 o2 = mats.instance[frag.instanceId].material.input2.xy;
    vec2 o3 = mats.instance[frag.instanceId].material.input2.zw;
    vec2 o4 = mats.instance[frag.instanceId].material.input3.xy;

    vec4 c1 = get_color_with_offset(0u, o1);
    vec4 c2 = get_color_with_offset(1u, o2);
    vec4 c3 = get_color_with_offset(2u, o3);
    vec4 c4 = get_color_with_offset(3u, o4);

    uint flags = uint(mats.instance[frag.instanceId].lightmap.meta1);
    uint flags2 = uint(mats.instance[frag.instanceId].lightmap.meta2);

    init_map_v2(out_color, c1, c2, flags & 0xFF);
//    combine_map_v2(out_color, c2, c3, (flags >> 8) & 0xFF);
    combine_map_v2(out_color, c3, c4, (flags >> 8)      & 0xFF);
//    combine_map_v2(out_color, c4, vec4(0), (flags >> 24) & 0xFF);

//    out_color = max(out_color, vec4(0.1));

    return vec4(out_color.rgb, out_color.a);
}

vec4 shader_chicago_extended()
{
    return shader_chicago();
//    return vec4(1, 0, 1, 1);
}

#else

vec4 shader_chicago()
{
    return vec4(shader_dummy().rgb, 1.0);
}

vec4 shader_chicago_extended()
{
    return vec4(shader_dummy().rgb, 1.0);
}

#endif

vec4 shader_transparent()
{
    return vec4(shader_dummy().rgb, 1.0);
}

vec4 shader_model()
{
    const uint multi_map_id  = 1u;
    const uint detail_map_id = 2u;

    float alpha_ref = 0.5;
    vec4 color = get_color(base_map_id);
    if(color.a < alpha_ref)
        discard;

    /* These are shamelessly stolen from the original shader */
    vec4 primary_change_color = vec4(1) - vec4(vec3(1), 0); // cb[0]
    vec4 fog_color_correction_0 = vec4(0, 0, 0, 1); // cb[1]
    vec4 fog_color_correction_E = vec4(0, 0, 0, 1); // cb[2]
    vec4 fog_color_correction_1 = vec4(0, 0, 0, 1); // cb[3]
    vec4 self_illum_color = vec4(0); // cb[4]
    vec4 fog_color = vec4(0); // cb[5]

    vec4 coloring = vec4(1) * primary_change_color;

    vec4 multi = get_color(multi_map_id).bgra;
    // HLSL does some vec4 -> vec2 cast here, so this might be wrong
    vec3 specular = vec3(multi.xz - multi.zw, 0); // add r3.xy, -r2.zwzz, r2.xzxx
    multi.xz = fog_color_correction_1.x * specular.xy + multi.zw; // mad r1.xyz, fog_color_correction_1.w, r3.xxyx, r2.zzwz
    specular.xyz = clamp(multi.y * self_illum_color.rgb + 1, 0, 1);
    coloring.rgb = coloring.rgb * multi.z + 1;
    float specular_contribution = 0.8;
    coloring.a = multi.r * specular_contribution;
    coloring.rgb = coloring.rgb * specular.xyz;

#if USE_REFLECTIONS == 1
    vec3 reflection = get_cube_color(view_direction()).rgb;
#else
    vec3 reflection = vec3(1);
#endif

    // mul r2.xyz, r2.xyz, v2.xyz
    reflection = reflection * coloring.a;
    color.rgb = clamp(color.rgb /** coloring.rgb*/ + reflection.rgb, 0, 1);

    vec4 detail = get_color(detail_map_id);
//    color.rgb = detail.rgb * 2 + color.rgb;
//    color.rgb = clamp(color.rgb - 1, 0, 1);

    return vec4(
        color.rgb *
        detail.rgb *
#if USE_REFLECTIONS == 1
//        mix(vec3(1), reflection, reflection_factor) *
#endif
        1, color.a);
}

vec4 shader_glass()
{
    return shader_dummy();
}

vec4 shader_meter()
{
    return vec4(shader_dummy().rgb, 1.0);
}

vec4 shader_plasma()
{
    return shader_dummy();
}

vec4 shader_water()
{
    const int ALPHA_MODULATES_REFLECT    = 0x1;
    const int COLOR_MODULATES_BACKGROUND = 0x2;

    const uint bump_map_id = 1u;

    int flags = mats.instance[frag.instanceId].material.flags & 0x3;

    vec4 base = get_color(base_map_id);
#if USE_REFLECTIONS == 1
    vec4 reflection = get_cube_color(view_direction());
#else
    vec4 reflection = vec4(1);
#endif
#if USE_NORMALMAP == 1
    float angle_rad = mats.instance[frag.instanceId].material.input1.x;
    vec2 angle = vec2(sin(angle_rad), cos(angle_rad));
    float velocity = mats.instance[frag.instanceId].material.input1.y;
    vec4 normal = get_bump(bump_map_id, angle * velocity * time);
#endif
    vec4 parallel = mats.instance[frag.instanceId].material.input2;
    vec4 perpendicular = mats.instance[frag.instanceId].material.input3;

    velocity = 0.001;
    vec3 normal_1 = get_bump(bump_map_id, vec2(sin(1), cos(1)) * (velocity + 0.001) * time).rgb;
    vec3 normal_2 = get_bump(bump_map_id, vec2(sin(0), cos(0)) * (velocity - 0.005) * time).rgb;
    vec3 normal_3 = get_bump(bump_map_id, vec2(sin(2), cos(2)) * (velocity + 0.005) * time).rgb;
    vec3 normal_4 = get_bump(bump_map_id, vec2(sin(3), cos(3)) * (velocity - 0.010) * time).rgb;

    /* TODO: This is very broken, does not look right */

    normal.xyz = normal_1 * normal_2 * normal_3 * normal_4;
    normal.w = dot(normal.xyz, -view_direction());

    float camera_angle = 1 - dot(normal.xyz, view_direction());

    if((flags & COLOR_MODULATES_BACKGROUND) != 0)
    {
//        reflection.rgb = reflection.rgb * base.rgb;
        reflection.a = base.a * normal.w;
    }
    if((flags & ALPHA_MODULATES_REFLECT) != 0)
        reflection.a = base.a *
            camera_angle *
            1;

    return vec4(
        max(mix(pow(reflection.rgb, vec3(8)),
            reflection.rgb,
            mix(parallel.rgb,
                perpendicular.rgb,
                (parallel.a + perpendicular.a) / 2)) *
        vec3(reflection.a), vec3(0)),
        reflection.a);
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
        final_color = shader_meter();
    } else if(material_id == MATERIAL_SOTR)
    {
        final_color = shader_transparent();
    } else
        final_color = vec4(0, 1, 0, 1);
}
