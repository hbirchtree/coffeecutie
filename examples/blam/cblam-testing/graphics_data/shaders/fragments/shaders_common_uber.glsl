#include "map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 position;
    vec2 tex;
    vec3 tangent;
    vec3 binormal;
    vec3 normal;
    vec2 light_tex;
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

struct FogProperties
{
    vec4 indoor_color;
    vec4 indoor_ambient;
    vec4 outdoor_color;
    vec4 outdoor_ambient;
    vec4 distances;
};

layout(binding = 2, std140) uniform WorldProperties
{
    LightProperties lighting[2];
    FogProperties fog;
} world;

const uint INTERIOR_LIGHTING = 0u;
const uint EXTERIOR_LIGHTING = 1u;

mat3 tbn_matrix()
{
    return mat3(frag.tangent, frag.binormal, frag.normal);
}

vec3 view_direction()
{
    return normalize(transpose(tbn_matrix()) * (camera_position - frag.position));
}
vec3 light_direction()
{
    vec3 direction = world.lighting[INTERIOR_LIGHTING].light_direction.xyz;
    return normalize(
        transpose(tbn_matrix()) *
        direction);
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
    return get_color_explicit_with_offset(map_id, layer, vec2(0));
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
    normal = /*tbn_matrix() **/ normal;
    return vec4(normal, dot(normal, light_direction()));
    // return vec4(normal, dot(normal, light_direction()));
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

    vec2 scroll_uv = mats.instance[frag.instanceId].material.input4.xy;
    vec4 base = get_color_with_offset(base_map_id, scroll_uv);
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
        vec3 view_world   = normalize(camera_position - frag.position);
        vec3 world_bump   = normalize(tbn_matrix() * normal.rgb);
        float NdotV       = clamp(dot(world_bump, view_world), 0.0, 1.0);
        vec3 reflect_dir  = reflect(-view_world, world_bump);
        reflection        = get_cube_color(reflect_dir).rgb;

        vec4 perp_color     = mats.instance[frag.instanceId].material.input2;
        vec4 parallel_color = mats.instance[frag.instanceId].material.input3;
        reflection_tint = mix(
            perp_color.rgb * perp_color.a,
            parallel_color.rgb * parallel_color.a,
            1.0 - NdotV);
    }
#endif
    vec3 out_color = base.rgb *
        micro.rgb *
        blend.rgb *
#if USE_LIGHTMAPS == 1
        lightmap.rgb *
#endif
#if USE_REFLECTIONS == 1
        mix(vec3(1), reflection * reflection_tint, 1.0 - base.a) *
#endif
#if USE_NORMALMAP == 1
  #if USE_LIGHTMAPS == 1
        // Half-Lambert ratio: both numerator and denominator in [0,2], never negative.
        // Flat surface => (L_ts.z+1)/(L_ts.z+1) = 1.0. Crevices < 1.0. Bumps > 1.0.
        clamp((normal.a + 1.0) / (light_direction().z + 1.0), 0.1, 2.0) *
  #else
        max(0.1, normal.a) *
  #endif
#endif
        vec3(1);

#if USE_SELF_ILLUMINATION == 1
    if(mats.instance[frag.instanceId].lightmap.meta1 != 0)
    {
        vec4 self_illum_tex  = get_color(micro_map_id);
        vec3 primary_color   = mats.instance[frag.instanceId].material.input5.rgb;
        vec3 secondary_color = mats.instance[frag.instanceId].material.input6.rgb;
        vec3 plasma_on_color = mats.instance[frag.instanceId].material.input7.rgb;
        float plasma_anim    = mats.instance[frag.instanceId].material.input7.a;
        // B = plasma mask, A = animation reference phase
        float plasma_prox    = self_illum_tex.b
            * max(0.0, 1.0 - abs(plasma_anim - self_illum_tex.a));
        out_color += self_illum_tex.r * primary_color
                   + self_illum_tex.g * secondary_color
                   + plasma_prox * plasma_on_color;
    }
#endif

    return vec4(out_color, 1.0);
}

#if USE_CHICAGO == 1

const uint F_CURRENT                     = 0u;
const uint F_NEXT                        = 1u;
const uint F_MUL                         = 2u;
const uint F_DOUBLE_MUL                  = 3u;
const uint F_ADD                         = 4u;
const uint F_ADD_SIGNED_CURRENT          = 5u;
const uint F_ADD_SIGNED_NEXT             = 6u;
const uint F_SUB_SIGNED_CURRENT          = 7u;
const uint F_SUB_SIGNED_NEXT             = 8u;
const uint F_BLEND_CURRENT_ALPHA         = 9u;
const uint F_BLEND_CURRENT_ALPHA_INVERSE = 10u;
const uint F_BLEND_NEXT_ALPHA            = 11u;
const uint F_BLEND_NEXT_ALPHA_INVERSE    = 12u;

// Stage 1+ combine: map1.color_func folds map (c3) into accumulated dst.
// F_CURRENT = no change, F_NEXT = replace with map, F_MUL = dst*map, etc.
void chicago_stage(out vec4 dst, in vec4 i1, in vec4 i2, in uint flags)
{
    uint cf = flags & 0xFu;
    uint af = (flags >> 4) & 0xFu;

    if(cf == F_CURRENT)
        dst.rgb = i1.rgb;
    else if(cf == F_NEXT)
        dst.rgb = i2.rgb;
    else if(cf == F_MUL)
        dst.rgb = i1.rgb * i2.rgb;
    else if(cf == F_DOUBLE_MUL)
        dst.rgb = clamp(i1.rgb * i2.rgb * i2.rgb, 0.0, 1.0);
    else if(cf == F_ADD)
        dst.rgb = i1.rgb + i2.rgb;
    else if(cf == F_ADD_SIGNED_CURRENT)
        dst.rgb = i1.rgb + i2.rgb - 0.5;
    else if(cf == F_ADD_SIGNED_NEXT)
        dst.rgb = i1.rgb + i2.rgb - 0.5;
    else if(cf == F_SUB_SIGNED_CURRENT)
        dst.rgb = i1.rgb - i2.rgb;
    else if(cf == F_SUB_SIGNED_NEXT)
        dst.rgb = i1.rgb - i2.rgb;
    else if(cf == F_BLEND_CURRENT_ALPHA)
        dst.rgb = mix(i1.rgb, i2.rgb, i1.a);
    else if(cf == F_BLEND_CURRENT_ALPHA_INVERSE)
        dst.rgb = mix(i1.rgb, i2.rgb, 1.0 - i1.a);
    else if(cf == F_BLEND_NEXT_ALPHA)
        dst.rgb = mix(i1.rgb, i2.rgb, i2.a);
    else if(cf == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.rgb = mix(i1.rgb, i2.rgb, 1.0 - i2.a);
    // F_CURRENT: no change

    if(af == F_CURRENT)
        dst.a = i1.a;
    else if(af == F_NEXT)
        dst.a = i2.a;
    else if(af == F_MUL)
        dst.a = i1.a * i2.a;
    else if(af == F_DOUBLE_MUL)
        dst.a = clamp(i1.a * i2.a * i2.a, 0.0, 1.0);
    else if(af == F_ADD)
        dst.a = i1.a + i2.a;
    else if(af == F_ADD_SIGNED_CURRENT)
        dst.a = i1.a + i2.a - 0.5;
    else if(af == F_ADD_SIGNED_NEXT)
        dst.a = i1.a + i2.a - 0.5;
    else if(af == F_SUB_SIGNED_CURRENT)
        dst.a = i1.a - i2.a;
    else if(af == F_SUB_SIGNED_NEXT)
        dst.a = i1.a - i2.a;
    else if(af == F_BLEND_CURRENT_ALPHA)
        dst.a = mix(i1.a, i2.a, i1.a);
    else if(af == F_BLEND_CURRENT_ALPHA_INVERSE)
        dst.a = mix(i1.a, i2.a, 1.0 - i1.a);
    else if(af == F_BLEND_NEXT_ALPHA)
        dst.a = mix(i1.a, i2.a, i2.a);
    else if(af == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.a = mix(i1.a, i2.a, 1.0 - i2.a);
    // F_CURRENT: no change
}

vec4 chicago_blend(vec4 c1, vec4 c2, vec4 c3, vec4 c4, uint flags)
{
    uint s0cf      = flags & 0xFu;
    vec4 out_color;
    chicago_stage(out_color, c1, c2, flags & 0xFFu);
    chicago_stage(out_color, out_color, c3, (flags >> 8) & 0xFFu);
    chicago_stage(out_color, out_color, c4, (flags >> 16) & 0xFFu);
    return out_color;
}

vec4 shader_chicago()
{
    vec2 o1 = mats.instance[frag.instanceId].material.input1.xy;
    vec2 o2 = mats.instance[frag.instanceId].material.input2.xy;
    vec2 o3 = mats.instance[frag.instanceId].material.input2.zw;

    vec4 c1 = get_color_with_offset(0u, o1);
    vec4 c2 = get_color_with_offset(1u, o2);
    vec4 c3 = get_color_with_offset(2u, o3);

    uint flags = uint(mats.instance[frag.instanceId].lightmap.meta1);
    return chicago_blend(c1, c2, vec4(0.0), vec4(0.0), flags);
}

vec4 shader_chicago_extended()
{
    vec2 o1 = mats.instance[frag.instanceId].material.input1.xy;
    vec2 o2 = mats.instance[frag.instanceId].material.input2.xy;
    vec2 o3 = mats.instance[frag.instanceId].material.input2.zw;
    vec2 o4 = mats.instance[frag.instanceId].material.input3.xy;

    vec4 c1 = get_color_with_offset(0u, o1);
    vec4 c2 = get_color_with_offset(1u, o2);
    vec4 c3 = get_color_with_offset(2u, o3);
    vec4 c4 = get_color_with_offset(3u, o4);

    uint flags = uint(mats.instance[frag.instanceId].lightmap.meta1);
    return chicago_blend(c1, c2, c3, c4, flags);
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
    vec4 color = shader_dummy();
//    return vec4(color.rgb, color.a);
    return color;
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
    // vec4 primary_change_color = vec4(1) - vec4(vec3(1), 0); // cb[0]
    vec4 fog_color_correction_0 = vec4(0, 0, 0, 1); // cb[1]
    vec4 fog_color_correction_E = vec4(0, 0, 0, 1); // cb[2]
    vec4 fog_color_correction_1 = vec4(0, 0, 0, 1); // cb[3]
    vec4 self_illum_color = vec4(0); // cb[4]
    vec4 fog_color = vec4(0); // cb[5]
    vec4 primary_change_color = mats.instance[frag.instanceId].material.input2;

    // vec4 coloring = vec4(1) * primary_change_color;

    vec4 multi = get_color(multi_map_id);

    // HLSL does some vec4 -> vec2 cast here, so this might be wrong
    // vec3 specular = vec3(multi.xy - multi.xw, 0); // add r3.xy, -r2.zwzz, r2.xzxx
    // multi.xz = fog_color_correction_1.x * specular.xy + multi.zw; // mad r1.xyz, fog_color_correction_1.w, r3.xxyx, r2.zzwz
    // specular.xyz = clamp(multi.z * self_illum_color.rgb + 1, 0, 1);
    // coloring.rgb = coloring.rgb * multi.z + 1;
    float specular_contribution = multi.w * 0.8;
    // coloring.a = multi.r * specular_contribution;
    // coloring.rgb = coloring.rgb * specular.xyz;

#if USE_REFLECTIONS == 1
    vec3 view_world = normalize(camera_position - frag.position);
    vec3 reflection = vec3(1);
    if((uint(mats.instance[frag.instanceId].lightmap.reflection) >> 24) != 0u)
    {
        float NdotV_m      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
        float fresnel_m    = 1.0 - NdotV_m;
        vec3 reflect_dir   = reflect(-view_world, frag.normal);
        vec4 perp_m        = mats.instance[frag.instanceId].material.input3;
        vec4 para_m        = mats.instance[frag.instanceId].material.input4;
        // Brightness controls blend strength; tint colors the reflection.
        // Keeping them separate prevents zero-brightness from darkening the base.
        float refl_strength = mix(perp_m.a, para_m.a, fresnel_m);
        vec3  refl_color    = mix(perp_m.rgb, para_m.rgb, fresnel_m);
        reflection = mix(vec3(1), get_cube_color(reflect_dir).rgb * refl_color, multi.w * refl_strength);
    }
#else
    vec3 reflection = vec3(1);
#endif

    // Diffuse lighting via geometry normal (soso has no bump map texture).
    // frag.normal is world-space and interpolated per-vertex (Gouraud approximation).
    vec3 world_light = normalize(world.lighting[INTERIOR_LIGHTING].light_direction.xyz);
    float NdotL = max(0.1, dot(frag.normal, world_light));
    color.rgb = clamp(color.rgb * NdotL
        /*+ coloring.rgb*/
        /*+ reflection.rgb*/, 0, 1);

    vec4 detail = get_color(detail_map_id);
//    color.rgb = detail.rgb * 2 + color.rgb;
//    color.rgb = clamp(color.rgb - 1, 0, 1);

    return vec4(
        // multi.rgb *
        mix(
            color.rgb,
            primary_change_color.rgb,
            multi.z * primary_change_color.a) *
        detail.rgb *
#if USE_REFLECTIONS == 1
       reflection *
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

    int  flags         = mats.instance[frag.instanceId].material.flags & 0x3;
    vec4 parallel      = mats.instance[frag.instanceId].material.input2;
    vec4 perpendicular = mats.instance[frag.instanceId].material.input3;
    vec4 base          = get_color(base_map_id);

    float angle_rad = mats.instance[frag.instanceId].material.input1.x;
    float velocity  = mats.instance[frag.instanceId].material.input1.y;

    // Four ripple layers at evenly-spaced angles, varying speeds.
    // get_bump returns tangent-space normals decoded to [-1, 1].
    vec3 n1 = get_bump(bump_map_id, vec2(cos(angle_rad),         sin(angle_rad))         * velocity        * time).xyz;
    vec3 n2 = get_bump(bump_map_id, vec2(cos(angle_rad + 1.047), sin(angle_rad + 1.047)) * velocity * 1.3  * time).xyz;
    vec3 n3 = get_bump(bump_map_id, vec2(cos(angle_rad + 2.094), sin(angle_rad + 2.094)) * velocity * 0.8  * time).xyz;
    vec3 n4 = get_bump(bump_map_id, vec2(cos(angle_rad + 3.14),  sin(angle_rad + 3.14))  * velocity * 0.6  * time).xyz;
    vec3 bump_ts = normalize(n1 + n2 + n3 + n4);

    // World-space quantities for reflection and Fresnel.
    // tbn_matrix() columns are T/B/N so it transforms tangent -> world.
    vec3 view_world   = normalize(camera_position - frag.position);
    vec3 world_normal = normalize(tbn_matrix() * bump_ts);

    float NdotV   = clamp(dot(world_normal, view_world), 0.0, 1.0);
    float fresnel = pow(1.0 - NdotV, 4.0);

    // Fresnel blend: perpendicular at normal incidence, parallel at grazing.
    // brightness (alpha channel) is the view-angle opacity, not a color scale.
    vec3  reflect_color = mix(perpendicular.rgb, parallel.rgb, fresnel);
    float out_alpha     = mix(perpendicular.a,   parallel.a,   fresnel);

#if USE_REFLECTIONS == 1
    vec3 reflect_dir  = reflect(-view_world, world_normal);
    reflect_color    *= get_cube_color(reflect_dir).rgb;
#endif

    if((flags & COLOR_MODULATES_BACKGROUND) != 0)
        reflect_color *= base.rgb;
    if((flags & ALPHA_MODULATES_REFLECT) != 0)
        out_alpha *= base.a;

    return vec4(reflect_color, out_alpha);
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
    vec4 color;

    if(material_id == MATERIAL_SENV)
    {
        color = shader_environment();
#if USE_MODEL_SHADERS == 1
    } else if(material_id == MATERIAL_SOSO)
    {
        color = shader_model();
#endif
    } else if(material_id == MATERIAL_SCHI)
    {
        final_color = shader_chicago();
        return;
    } else if(material_id == MATERIAL_SCEX)
    {
        final_color = shader_chicago_extended();
        return;
    } else if(material_id == MATERIAL_SWAT)
    {
        color = shader_water();
    } else if(material_id == MATERIAL_SGLA)
    {
        color = shader_glass();
    } else if(material_id == MATERIAL_SMET)
    {
        color = shader_meter();
    } else if(material_id == MATERIAL_SOTR)
    {
        final_color = shader_transparent();
        return;
    } else
        color = vec4(0, 1, 0, 1);


    vec4 fog_color = vec4(world.fog.outdoor_color.xyz, 1);
    float fog_distance = length(frag.position - camera_position);
    fog_distance = (fog_distance - world.fog.distances.z) / world.fog.distances.w;
    fog_distance = clamp(exp(-fog_distance * world.fog.outdoor_color.w), 0, 1);
    final_color = mix(
        color,
        fog_color,
        1 - fog_distance);
}
