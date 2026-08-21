/* Use the format-aware get_light defined below (samples the correct source_*
 * bucket per lightmap format) instead of map_sampling.glsl's RGB565-only one. */
#define LIGHTMAP_FORMAT_AWARE 1
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
// TODO: Add constant fallback on release mode
layout(location = 31) uniform int render_flags;

const int RENDER_FLAG_FOG                = 0x1;
const int RENDER_FLAG_LIGHTMAP           = 0x2;
const int RENDER_FLAG_REFLECTION         = 0x4;
// 0x8 taken by a vertex shader flag
const int RENDER_FLAG_ONLY_NORMALS       = 0x10;
const int RENDER_FLAG_ONLY_NORMALMAP     = 0x20;
const int RENDER_FLAG_ONLY_LIGHTMAP      = 0x40;
const int RENDER_FLAG_ONLY_REFLECTIONS   = 0x80;
const int RENDER_FLAG_ONLY_MULTIPURPOSE  = 0x100;
const int RENDER_FLAG_ONLY_MULTIPURPOSE2 = 0x200;
const int RENDER_FLAG_ONLY_DIFFUSE       = 0x400;
const int RENDER_FLAG_CUBE_YAW           = 0x800;
const int RENDER_FLAG_ONLY_DETAIL        = 0x1000;
const int RENDER_FLAG_ONLY_MICRO         = 0x2000;
const int RENDER_FLAG_ONLY_AUX           = 0x4000;
const int RENDER_FLAG_INTERIOR           = 0x8000;

const uint MATERIAL_FLAG_INTERIOR        = 0x10000u;

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

const int MAX_WORLD_LIGHTS = 8;

layout(binding = 2, std140) uniform WorldProperties
{
    LightProperties lighting[MAX_WORLD_LIGHTS];
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
vec3 world_key_light()
{
    uint want = (render_flags & RENDER_FLAG_INTERIOR) != 0 ? 2u : 1u;
    for(int li = 0; li < MAX_WORLD_LIGHTS; li++)
    {
        vec4 dir = world.lighting[li].light_direction;
        if(dot(dir.xyz, dir.xyz) < 1e-6)
            continue;
        if((uint(dir.w) & want) != 0u)
            return dir.xyz;
    }
    return world.lighting[0].light_direction.xyz;
}

vec3 light_direction()
{
    return normalize(transpose(tbn_matrix()) * world_key_light());
}

vec4 sample_map(
    in uint map_id,
    in int layer,
    in sampler2DArray sampler,
    in vec2 offset,
    in Material mat)
{
    return get_map(map_id,
        layer,
        sampler,
        frag.tex + offset,
        mat);
}

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGB565 = 4u;
const uint TEX_R8     = 5u;
const uint TEX_RG8    = 6u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;
/* Xbox luminance/alpha semantics stored in R8/RG8 buckets */
const uint TEX_A8     = 9u;  /* 000A */
const uint TEX_Y8     = 10u; /* LLL1 */
const uint TEX_AY8    = 11u; /* LLLL */
const uint TEX_A8Y8   = 12u; /* LLLA (r=L, g=A) */
const uint TEX_P8     = 13u; /* Xbox palettized bump, index in R8 */

vec4 get_color_explicit_with_offset(in uint map_id, in int tex_id, in vec2 offset, in Material mat)
{
    uint source = tex_id >> 24;
    if(source == TEX_BC1)
        return sample_map(map_id, tex_id, source_bc1, offset, mat);
    else if(source == TEX_BC2)
        return sample_map(map_id, tex_id, source_bc2, offset, mat);
    else if(source == TEX_BC3)
        return sample_map(map_id, tex_id, source_bc3, offset, mat);
    else if(source == TEX_R8)
        return sample_map(map_id, tex_id, source_r8, offset, mat);
    else if(source == TEX_RG8)
        return sample_map(map_id, tex_id, source_rg8, offset, mat);
    else if(source == TEX_A8)
        /* D3D A8 samples RGB as 1 (white), alpha from the texture — the
         * cloud mask multiplies onto the cloud color this way. */
        return vec4(1, 1, 1, sample_map(map_id, tex_id, source_r8, offset, mat).r);
    else if(source == TEX_Y8)
        return vec4(vec3(sample_map(map_id, tex_id, source_r8, offset, mat).r), 1);
    else if(source == TEX_AY8)
    {
        float l = sample_map(map_id, tex_id, source_r8, offset, mat).r;
        return vec4(vec3(l), l);
    }
    else if(source == TEX_A8Y8)
    {
        vec2 la = sample_map(map_id, tex_id, source_rg8, offset, mat).rg;
        return vec4(vec3(la.r), la.g);
    }
    else if(source == TEX_P8)
    {
        float i = sample_map(map_id, tex_id, source_r8, offset, mat).r;
        return vec4(vec3(i), i);
    }
    else if(source == TEX_RGB565)
        return sample_map(map_id, tex_id, source_rgb565, offset, mat).bgra;
    else if(source == TEX_RGBA4)
        return sample_map(map_id, tex_id, source_rgba4, offset, mat).bgra;
    else if(source == TEX_RGBA8)
        return sample_map(map_id, tex_id, source_rgba8, offset, mat).bgra;
    return vec4(vec3(1), 1);
}

vec4 get_color_with_offset(in uint map_id, in vec2 offset, in Material mat)
{
    int tex_id = mat.maps[map_id].layer;
    return get_color_explicit_with_offset(map_id, tex_id, offset, mat);
}

vec4 get_color(in uint map_id, in Material mat)
{
    return get_color_with_offset(map_id, vec2(0), mat);
}

vec4 get_color_explicit(in uint map_id, in int layer, in Material mat)
{
    return get_color_explicit_with_offset(map_id, layer, vec2(0), mat);
}

#if USE_LIGHTMAPS == 1
/* Format-aware lightmap fetch: the lightmap's format is encoded in the high
 * byte of its layer (same scheme as get_color). Stock maps use RGB565
 * lightmaps; HD/Refined maps store them as RGBA8. Sample the matching bucket
 * instead of the old hardcoded RGB565-only `lightmaps` sampler. */
vec4 get_light(in Material mat, in vec2 light_tex)
{
    vec2 light_scale  = mat.lightmap.atlas_scale;
    vec2 light_offset = mat.lightmap.atlas_offset;
    int  tex_id       = mat.lightmap.layer;
    vec2 uv           = light_tex * light_scale + light_offset;
    int  layer        = tex_id & 0xFFFF;
    uint source       = uint(tex_id) >> 24;

    if(source == 0u)
        return vec4(1.0);
    if(source == TEX_RGBA8)
        return texture(source_rgba8, vec3(uv, layer), -100.0).bgra;
    return texture(source_rgb565, vec3(uv, layer), -100.0);
}
#endif

#if USE_REFLECTIONS == 1
vec3 world_to_cube(in vec3 v)
{
    if((render_flags & RENDER_FLAG_CUBE_YAW) != 0)
        v = vec3(v.y, -v.x, v.z);
    return vec3(-v.y, v.z, v.x);
}

vec4 get_cube_color(in vec3 world_dir, in Material mat)
{
    vec3 tex_coord = world_to_cube(world_dir);
    int tex_id = mat.lightmap.reflection;
    uint source = tex_id >> 24;
    if(source == TEX_BC1)
        return get_cube_map(source_cube_bc1, tex_coord, mat);
    else if(source == TEX_RGB565)
        return get_cube_map(source_cube_rgb565, tex_coord, mat);
    else if(source == TEX_RGBA8)
        return get_cube_map(source_cube_rgba8, tex_coord, mat);
    else
        return vec4(1);
}
#else
vec4 get_cube_color(in vec3 tex_coord, in Material mat)
{
    return vec4(1);
}
#endif

const float P8_BUMP_GAIN = 1.0;

vec4 get_bump(in uint map_id, in vec2 offset, in Material mat)
{
    uint source = uint(mat.maps[map_id].layer) >> 24;
    vec3 normal;
    if(source == TEX_P8)
    {
        /* P8 bump maps index a palette that lives in the game executable, not
         * in the map. Without it, read the index as a height and take the
         * slope across a texel — approximate, but it keeps the surface lit
         * like a flat one instead of normalizing an index into nonsense. */
        vec2 texel = 1.0 / vec2(textureSize(source_r8, 0).xy);
        vec2 step  = texel / max(
            mat.maps[map_id].uv_scale * mat.maps[map_id].atlas_scale,
            vec2(1e-6));
        float here = get_color_with_offset(map_id, offset, mat).r;
        float du = get_color_with_offset(map_id, offset + vec2(step.x, 0), mat).r;
        float dv = get_color_with_offset(map_id, offset + vec2(0, step.y), mat).r;
        normal = normalize(vec3(
            (here - du) * P8_BUMP_GAIN, (here - dv) * P8_BUMP_GAIN, 1.0));
    } else
        normal = normalize(get_color_with_offset(map_id, offset, mat).rgb * 2.0 - 1.0);
    return vec4(normal, dot(normal, light_direction()));
}

const uint base_map_id      = 0u;

const uint DETAIL_BIASED_MULTIPLY = 0u;
const uint DETAIL_MULTIPLY        = 1u;
const uint DETAIL_BIASED_ADD      = 2u;

vec3 apply_detail(in vec3 color, in vec3 detail, in uint func)
{
    if(func == DETAIL_MULTIPLY)
        return color * detail;
    if(func == DETAIL_BIASED_ADD)
        return clamp(color + detail * 2.0 - 1.0, 0.0, 1.0);
    return clamp(color * detail * 2.0, 0.0, 1.0);
}

vec4 shader_dummy(in Material mat)
{
    return get_color(base_map_id, mat);
}

vec4 shader_environment(in Material mat)
{
    const int TYPE_NORMAL           = 0;
    const int TYPE_BLENDED          = 1;
    const int TYPE_BLENDED_SPECULAR = 2;

    const uint micro_map_id     = 1u;
    const uint primary_map_id   = 2u;
    const uint secondary_map_id = 3u;
    const uint bump_map_id      = 4u;

    const uint SENV_FLAG_ALPHA_TESTED = 0x1u;

    uint flags = uint(mat.material.flags & 0x7);
    uint type = uint((mat.material.flags >> 4) & 0x3);

    int has_micro = (mat.material.flags >> 10) & 0x1;
    uint detail_func = uint((mat.material.flags >> 20) & 0x3);
    uint micro_func  = uint((mat.material.flags >> 22) & 0x3);

    bool has_primary   = (uint(mat.maps[primary_map_id].layer) >> 24) != 0u;
    bool has_secondary = (uint(mat.maps[secondary_map_id].layer) >> 24) != 0u;
    bool has_bump      = (uint(mat.maps[bump_map_id].layer) >> 24) != 0u;

    vec2 scroll_uv = mat.material.input4.xy;
    vec4 base = get_color_with_offset(base_map_id, scroll_uv, mat);
    vec4 micro = has_micro == 1 ? get_color(micro_map_id, mat) : vec4(1);
    vec4 primary = get_color(primary_map_id, mat);
    vec4 secondary = get_color(secondary_map_id, mat);

    if((flags & SENV_FLAG_ALPHA_TESTED) != 0u && has_bump)
    {
        vec4  bump_texel = get_color(bump_map_id, mat);
        bool  is_p8      = (uint(mat.maps[bump_map_id].layer) >> 24) == TEX_P8;
        float cutout     = is_p8 ? 1.0 - bump_texel.r : bump_texel.a;
        if(cutout < 0.05)
            discard;
    }

    if((render_flags & RENDER_FLAG_ONLY_DIFFUSE) != 0)
        return vec4(base.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE) != 0)
        return vec4(primary.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE2) != 0)
        return vec4(secondary.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MICRO) != 0)
        return vec4(micro.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_AUX) != 0)
        return vec4(base.a, secondary.a, micro.a, 1);
#if USE_LIGHTMAPS == 1
    vec4 lightmap = (render_flags & RENDER_FLAG_LIGHTMAP) != 0
        ? get_light(mat, frag.light_tex)
        : vec4(1.0);
    if((render_flags & RENDER_FLAG_ONLY_LIGHTMAP) != 0)
        return lightmap;
#endif

    float factor = type == TYPE_NORMAL ? secondary.a : base.a;
    vec4 blend = vec4(1);
    if(has_primary && has_secondary)
        blend = (primary * factor) + (secondary * (1.0 - factor));
    else if(has_primary)
        blend = primary;
    else if(has_secondary)
        blend = secondary;
    float specular = type == TYPE_BLENDED ? micro.a : base.a;

#if USE_NORMALMAP == 1
    vec4 normal = has_bump
        ? get_bump(bump_map_id, vec2(0), mat)
        : vec4(0, 0, 1, dot(vec3(0, 0, 1), light_direction()));
#endif

#if USE_REFLECTIONS == 1
    int reflective = (mat.material.flags >> 6) & 0x1;
    vec3 reflection   = vec3(1.0);
    vec3 refl_color   = vec3(1.0);
    float refl_strength = 0.0;
    if(reflective > 0 && (render_flags & RENDER_FLAG_REFLECTION) != 0)
    {
        uint reflect_type = uint((mat.material.flags >> 7) & 0x3);
        float lightmap_brightness = mat.material.input1.x;

        vec3 view_world  = normalize(camera_position - frag.position);
        vec3 surf_normal = normalize(frag.normal);
#if USE_NORMALMAP == 1
        vec3 world_bump  = normalize(tbn_matrix() * normal.rgb);
#else
        vec3 world_bump  = surf_normal;
#endif

        // bumped_cube=0: perturbed normal; flat_cube=1: geometric normal; bumped_radiosity=2: bumped
        vec3 refl_normal = (reflect_type == 1u) ? surf_normal : world_bump;
        float NdotV      = clamp(dot(refl_normal, view_world), 0.0, 1.0);
        vec3 reflect_dir = reflect(-view_world, refl_normal);
        reflection       = get_cube_color(reflect_dir, mat).rgb;

        vec4 perp_color     = mat.material.input2;
        vec4 parallel_color = mat.material.input3;
        float t   = 1.0 - NdotV; // perp=grazing(NdotV=0), parallel=straight-on(NdotV=1)
        refl_color    = mix(parallel_color.rgb, perp_color.rgb, t);
        refl_strength = mix(parallel_color.a,   perp_color.a,   t);

#if USE_LIGHTMAPS == 1
        float lm_luma   = dot(lightmap.rgb, vec3(0.299, 0.587, 0.114));
        refl_strength  *= mix(0.5 + 0.5 * lm_luma, 1.0, lightmap_brightness);
#endif
    }
#endif
#if USE_NORMALMAP == 1
    float light_z     = light_direction().z;
    float bump_factor = clamp(
        1.0 + (normal.a - light_z) / max(light_z + 1.0, 0.5), 0.1, 2.0);
    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
        return vec4(normalize(frag.normal), 1);
    if((render_flags & RENDER_FLAG_ONLY_NORMALMAP) != 0)
        return vec4(normal.rgb, 1);
#endif
    vec3 out_color = base.rgb;
    if(has_primary || has_secondary)
        out_color = apply_detail(out_color, blend.rgb, detail_func);
    if(has_micro == 1)
        out_color = apply_detail(out_color, micro.rgb, micro_func);
#if USE_LIGHTMAPS == 1
    out_color *= lightmap.rgb;
#endif
#if USE_REFLECTIONS == 1
    vec3 refl_out =
        reflection * refl_color * refl_strength * specular * 2.0;
    if((render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0)
        return vec4(refl_out, 1);
    out_color = clamp(out_color + refl_out, 0.0, 1.0);
#endif
#if USE_NORMALMAP == 1
    out_color *= bump_factor;
#endif

#if USE_SELF_ILLUMINATION == 1
    if(mat.lightmap.meta1 != 0)
    {
        vec4 self_illum_tex  = get_color(micro_map_id, mat);
        vec3 primary_color   = mat.material.input5.rgb;
        vec3 secondary_color = mat.material.input6.rgb;
        vec3 plasma_on_color = mat.material.input7.rgb;
        float plasma_anim    = mat.material.input7.a;
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

void chicago_stage(out vec4 dst, in vec4 i1, in vec4 last, in vec4 i2, in uint flags)
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
        dst.rgb = i1.rgb * i2.rgb * 2;
    else if(cf == F_ADD || cf == F_ADD_SIGNED_CURRENT || cf == F_ADD_SIGNED_NEXT)
        dst.rgb = i1.rgb + i2.rgb;
    else if(cf == F_SUB_SIGNED_CURRENT || cf == F_SUB_SIGNED_NEXT)
        dst.rgb = i1.rgb - i2.rgb;
    else if(cf == F_BLEND_CURRENT_ALPHA)
        dst.rgb = i1.rgb + i2.rgb * last.a;
    else if(cf == F_BLEND_CURRENT_ALPHA_INVERSE)
        dst.rgb = i1.rgb + i2.rgb * (1 - last.a);
    else if(cf == F_BLEND_NEXT_ALPHA)
        dst.rgb = i1.rgb + i2.rgb * i2.a;
    else if(cf == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.rgb = i1.rgb + i2.rgb * (1 - i2.a);

    if(af == F_CURRENT)
        dst.a = i1.a;
    else if(af == F_NEXT)
        dst.a = i1.a;
    else if(af == F_MUL)
        dst.a = i1.a * i2.a;
    else if(af == F_DOUBLE_MUL)
        dst.a = i1.a * i2.a * 2;
    else if(af == F_ADD || af == F_ADD_SIGNED_CURRENT || af == F_ADD_SIGNED_NEXT)
        dst.a = i1.a + i2.a;
    else if(af == F_SUB_SIGNED_CURRENT || af == F_SUB_SIGNED_NEXT)
        dst.a = i1.a - i2.a;
    else if(af == F_BLEND_CURRENT_ALPHA)
        dst.a = i1.a * last.a;
    else if(af == F_BLEND_CURRENT_ALPHA_INVERSE)
        dst.a = i1.a * (1 - last.a);
    else if(af == F_BLEND_NEXT_ALPHA)
        dst.a = i1.a + i2.a * last.a;
    else if(af == F_BLEND_NEXT_ALPHA_INVERSE)
        dst.a = i1.a + i2.a * (1 - i2.a);
}

vec4 chicago_blend(vec4 c1, vec4 c2, vec4 c3, vec4 c4, uint flags, in Material mat)
{
    vec4 out_color = vec4(1.0);
    chicago_stage(out_color, c1, c1, c2, flags & 0xFFu);
    if((uint(mat.maps[2].layer) >> 24) != 0u)
        chicago_stage(out_color, out_color, c2, c3, (flags >> 8) & 0xFFu);
    if((uint(mat.maps[3].layer) >> 24) != 0u)
        chicago_stage(out_color, out_color, c3, c4, (flags >> 16) & 0xFFu);
    return out_color;
}

vec4 shader_chicago(in Material mat)
{
    /* schi supports up to 4 maps just like scex — the Xbox sky dome chains
     * star detail + a blue gradient in maps 2+, which chicago_blend gates on
     * their layer being assigned. */
    vec2 o1 = mat.material.input1.xy;
    vec2 o2 = mat.material.input2.xy;
    vec2 o3 = mat.material.input2.zw;
    vec2 o4 = mat.material.input3.xy;

    vec4 c1 = get_color_with_offset(0u, o1, mat);
    vec4 c2 = get_color_with_offset(1u, o2, mat);
    vec4 c3 = get_color_with_offset(2u, o3, mat);
    vec4 c4 = get_color_with_offset(3u, o4, mat);

    uint flags = uint(mat.lightmap.meta1);
    return chicago_blend(c1, c2, c3, c4, flags, mat);
}

vec4 shader_chicago_extended(in Material mat)
{
    vec2 o1 = mat.material.input1.xy;
    vec2 o2 = mat.material.input2.xy;
    vec2 o3 = mat.material.input2.zw;
    vec2 o4 = mat.material.input3.xy;

    vec4 c1 = get_color_with_offset(0u, o1, mat);
    vec4 c2 = get_color_with_offset(1u, o2, mat);
    vec4 c3 = get_color_with_offset(2u, o3, mat);
    vec4 c4 = get_color_with_offset(3u, o4, mat);

    uint flags = uint(mat.lightmap.meta1);
    return chicago_blend(c1, c2, c3, c4, flags, mat);
}

#else

vec4 shader_chicago(in Material mat)
{
    return vec4(shader_dummy(mat).rgb, 1.0);
}

vec4 shader_chicago_extended(in Material mat)
{
    return vec4(shader_dummy(mat).rgb, 1.0);
}

#endif

/* Input mapping (shader_transparent::mapping_t) — NV register-combiner
 * input mappings. u = max(0,x) is the "unsigned" clamp the hardware applies. */
vec3 sotr_cmap(vec3 v, uint m)
{
    vec3 u = max(v, 0.0);
    if(m == 1u) return 1.0 - clamp(v, 0.0, 1.0); // one_minus_clamp (unsigned invert)
    if(m == 2u) return 2.0 * u - 1.0;            // two (expand normal)
    if(m == 3u) return 1.0 - 2.0 * u;            // one_minus_two (expand negate)
    if(m == 4u) return u - 0.5;                  // clamp_minus_half (half bias)
    if(m == 5u) return 0.5 - u;                  // half_minus_clamp (half bias negate)
    if(m == 6u) return v;                        // passthrough (signed identity)
    if(m == 7u) return -v;                       // negative (signed negate)
    return u;                                    // clamp (unsigned identity)
}
float sotr_smap(float v, uint m)
{
    float u = max(v, 0.0);
    if(m == 1u) return 1.0 - clamp(v, 0.0, 1.0);
    if(m == 2u) return 2.0 * u - 1.0;
    if(m == 3u) return 1.0 - 2.0 * u;
    if(m == 4u) return u - 0.5;
    if(m == 5u) return 0.5 - u;
    if(m == 6u) return v;
    if(m == 7u) return -v;
    return u;
}

/* Apply output mapping (output_mapping_t) */
vec3 sotr_omap(vec3 v, uint m)
{
    if(m == 1u) return v * 0.5;
    if(m == 2u) return v * 2.0;
    if(m == 3u) return v * 4.0;
    if(m == 4u) return v - 0.5;
    if(m == 5u) return v * 2.0 - 1.0;
    return v;
}
float sotr_somap(float v, uint m)
{
    if(m == 1u) return v * 0.5;
    if(m == 2u) return v * 2.0;
    if(m == 3u) return v * 4.0;
    if(m == 4u) return v - 0.5;
    if(m == 5u) return v * 2.0 - 1.0;
    return v;
}

/* Combiner register file (color_output destinations / inputs):
 * reg[1]=scratch_color_0 (final), reg[2]=scratch_color_1,
 * reg[3]=vertex_color_0, reg[4]=vertex_color_1, reg[5..8]=map_color_0..3.
 * Stages may WRITE any of these (incl. vertex/map registers) and later
 * stages read them back. reg[0] is the discard sink.
 * Vertex registers initialize to 1 — the engine substitutes fade factors
 * there and "no fade" is 1, not 0 (0 nukes any multiply stage to black). */

/* Get color input (shader_transparent::input_t) as vec3 */
vec3 sotr_cin(uint i, vec4 reg[9], vec4 c0, vec4 c1)
{
    if(i ==  1u) return vec3(1);
    if(i ==  2u) return vec3(0.5);
    if(i ==  3u) return vec3(-1);
    if(i ==  4u) return vec3(-0.5);
    if(i >=  5u && i <= 8u)  return reg[i].rgb;        // map_color0..3
    if(i ==  9u || i == 10u) return reg[i - 6u].rgb;   // vertex_color0/1
    if(i == 11u || i == 12u) return reg[i - 10u].rgb;  // scratch_color0/1
    if(i == 13u) return c0.rgb;   // constant_color0
    if(i == 14u) return c1.rgb;   // constant_color1
    if(i >= 15u && i <= 18u) return vec3(reg[i - 10u].a); // map_alpha0..3
    if(i == 19u || i == 20u) return vec3(reg[i - 16u].a); // vertex_alpha0/1
    if(i == 21u || i == 22u) return vec3(reg[i - 20u].a); // scratch_alpha0/1
    if(i == 23u) return vec3(c0.a); // constant_alpha0
    if(i == 24u) return vec3(c1.a); // constant_alpha1
    return vec3(0); // zero
}

/* Get alpha input (blam::shader::color_input).
 * NOTE different enum from color: 5-8 map alpha, 15-18 map BLUE,
 * 19/20 vertex blue, 21/22 scratch BLUE, 23/24 constant BLUE. */
float sotr_ain(uint i, vec4 reg[9], vec4 c0, vec4 c1)
{
    if(i ==  1u) return 1.0;
    if(i ==  2u) return 0.5;
    if(i ==  3u) return -1.0;
    if(i ==  4u) return -0.5;
    if(i >=  5u && i <= 8u)  return reg[i].a;          // map_alpha_0..3
    if(i ==  9u || i == 10u) return reg[i - 6u].a;     // vertex_alpha_0/1
    if(i == 11u || i == 12u) return reg[i - 10u].a;    // scratch_alpha_0/1
    if(i == 13u) return c0.a;
    if(i == 14u) return c1.a;
    if(i >= 15u && i <= 18u) return reg[i - 10u].b;    // map_blue_0..3
    if(i == 19u || i == 20u) return reg[i - 16u].b;    // vertex_blue_0/1
    if(i == 21u || i == 22u) return reg[i - 20u].b;    // scratch_blue_0/1
    if(i == 23u) return c0.b;    // constant_blue_0
    if(i == 24u) return c1.b;    // constant_blue_1
    return 0.0;
}

vec4 shader_transparent(in Material mat)
{
    /* Register file: 0=discard sink, 1/2=scratch, 3/4=vertex (init 1 =
     * "no fade"), 5..8=map colors. All writable, per NV combiner rules. */
    /* Per-map UV scroll offsets, animated CPU-side (input2 = maps 0/1,
     * input3 = maps 2/3) */
    vec4 uv01 = mat.material.input2;
    vec4 uv23 = mat.material.input3;

    TransparentData trd = tr.instance[frag.instanceId];

    vec4 reg[9];
    reg[0] = vec4(0);
    reg[1] = vec4(0);
    reg[2] = vec4(0);
    reg[3] = vec4(1);
    reg[4] = vec4(1);
    reg[5] = get_color_with_offset(0u, uv01.xy, mat);
    reg[6] = get_color_with_offset(1u, uv01.zw, mat);
    reg[7] = get_color_with_offset(2u, uv23.xy, mat);
    reg[8] = get_color_with_offset(3u, uv23.zw, mat);

    int num_stages = int(trd.num_stages);
    if(num_stages == 0)
        return reg[5];

    for(int si = 0; si < num_stages && si < 7; si++)
    {
        TransparentStage s = trd.stages[si];
        uint cin  = s.color_in;
        uint ain  = s.alpha_in;
        uint outs = s.outputs;

        /* Decode color inputs */
        uint ca_i = cin & 0x1Fu,        ca_m = (cin >> 5)  & 7u;
        uint cb_i = (cin >> 8)  & 0x1Fu, cb_m = (cin >> 13) & 7u;
        uint cc_i = (cin >> 16) & 0x1Fu, cc_m = (cin >> 21) & 7u;
        uint cd_i = (cin >> 24) & 0x1Fu, cd_m = (cin >> 29) & 7u;

        /* Decode alpha inputs */
        uint aa_i = ain & 0x1Fu,        aa_m = (ain >> 5)  & 7u;
        uint ab_i = (ain >> 8)  & 0x1Fu, ab_m = (ain >> 13) & 7u;
        uint ac_i = (ain >> 16) & 0x1Fu, ac_m = (ain >> 21) & 7u;
        uint ad_i = (ain >> 24) & 0x1Fu, ad_m = (ain >> 29) & 7u;

        /* Decode outputs: color occupies bits [0..16] (17 bits — the output
         * map's top bit is bit 16), alpha starts at bit 17. */
        uint c_ab_d  = outs & 0xFu,        c_ab_fn = (outs >> 4) & 1u;
        uint c_cd_d  = (outs >> 5) & 0xFu,  c_cd_fn = (outs >> 9) & 1u;
        uint c_sum_d = (outs >> 10) & 0xFu, c_om   = (outs >> 14) & 7u;
        uint a_ab_d  = (outs >> 17) & 0xFu;
        uint a_cd_d  = (outs >> 21) & 0xFu;
        uint a_sum_d = (outs >> 25) & 0xFu, a_om   = (outs >> 29) & 7u;

        /* The hardware reads every input from the register state at stage
         * entry; outputs land after. Snapshot so color writes don't feed
         * this stage's alpha inputs. */
        vec4 inr[9];
        for(int r = 0; r < 9; r++)
            inr[r] = reg[r];

        vec4 c0 = s.color0;
        vec4 c1 = s.color1;

        /* Color: get + map inputs */
        vec3 ca = sotr_cmap(sotr_cin(ca_i, inr, c0, c1), ca_m);
        vec3 cb = sotr_cmap(sotr_cin(cb_i, inr, c0, c1), cb_m);
        vec3 cc = sotr_cmap(sotr_cin(cc_i, inr, c0, c1), cc_m);
        vec3 cd = sotr_cmap(sotr_cin(cd_i, inr, c0, c1), cd_m);

        vec3 c_ab = c_ab_fn == 0u ? ca * cb : vec3(dot(ca, cb));
        vec3 c_cd = c_cd_fn == 0u ? cc * cd : vec3(dot(cc, cd));

        /* mux_sum destination: sum by default; with the color_mux/alpha_mux
         * stage flag it's the combiner MUX, selecting CD when r0.a ≥ 0.5,
         * else AB. Summing where mux was meant over-brightens badly. */
        bool color_mux = (s.flags & 1u) != 0u;
        bool alpha_mux = (s.flags & 2u) != 0u;
        vec3 c_sum =
            color_mux ? (inr[1].a >= 0.5 ? c_cd : c_ab) : (c_ab + c_cd);

        /* Route color outputs; registers clamp to [-1, 1] on write. */
        if(c_ab_d <= 8u)
            reg[c_ab_d].rgb = clamp(sotr_omap(c_ab, c_om), -1.0, 1.0);
        if(c_cd_d <= 8u)
            reg[c_cd_d].rgb = clamp(sotr_omap(c_cd, c_om), -1.0, 1.0);
        if(c_sum_d <= 8u)
            reg[c_sum_d].rgb = clamp(sotr_omap(c_sum, c_om), -1.0, 1.0);
        reg[0] = vec4(0); /* keep the discard sink discarded */

        /* Alpha: get + map inputs (from the stage-entry snapshot) */
        float aa = sotr_smap(sotr_ain(aa_i, inr, c0, c1), aa_m);
        float ab = sotr_smap(sotr_ain(ab_i, inr, c0, c1), ab_m);
        float ac = sotr_smap(sotr_ain(ac_i, inr, c0, c1), ac_m);
        float ad = sotr_smap(sotr_ain(ad_i, inr, c0, c1), ad_m);

        float a_ab = aa * ab;
        float a_cd = ac * ad;
        float a_sum =
            alpha_mux ? (inr[1].a >= 0.5 ? a_cd : a_ab) : (a_ab + a_cd);

        if(a_ab_d <= 8u)
            reg[a_ab_d].a = clamp(sotr_somap(a_ab, a_om), -1.0, 1.0);
        if(a_cd_d <= 8u)
            reg[a_cd_d].a = clamp(sotr_somap(a_cd, a_om), -1.0, 1.0);
        if(a_sum_d <= 8u)
            reg[a_sum_d].a = clamp(sotr_somap(a_sum, a_om), -1.0, 1.0);
        reg[0] = vec4(0);
    }

    vec4 sc0 = reg[1];

    /* blend_mode (chicago::framebuffer_blending):
     * 0=alpha_blend 1=multiply 2=double_multiply 3=add 4=subtract
     * 5=component_min 6=component_max 7=alpha_multiply_add
     * The engine's additive blend is (SRC_ALPHA, ONE), but Halo's true `add`
     * and component_max are alpha-independent. Force alpha=1 there so the
     * (SRC_ALPHA, ONE) state becomes straight additive (ONE, ONE). */
    uint bm = trd.blend_mode;
    if(bm == 3u || bm == 5u || bm == 6u)
        sc0.a = 1.0;
    return sc0;
}

const uint SOSO_FLAG_DETAIL_AFTER_REFLECTION      =   0x1;
const uint SOSO_FLAG_DETAIL_FUNC_MUL              =   0x2;
const uint SOSO_FLAG_DETAIL_FUNC_DOUBLE_BIAS_ADD  =   0x4;
const uint SOSO_FLAG_DETAIL_MASK_REFLECTION       =   0x8;
const uint SOSO_FLAG_DETAIL_MASK_REFLECTION_INV   =  0x10;
const uint SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR     =  0x20;
const uint SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR_INV =  0x40;
const uint SOSO_FLAG_DETAIL_MASK_SELF_ILLUM       =  0x80;
const uint SOSO_FLAG_DETAIL_MASK_SELF_ILLUM_INV   = 0x100;
const uint SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA      = 0x200;
const uint SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA_INV  = 0x400;

vec4 shader_model(in Material mat)
{
    const uint multi_map_id  = 1u;
    const uint detail_map_id = 2u;

    float alpha_ref = 0.5;
    vec4 color = get_color(base_map_id, mat);
    if(color.a < alpha_ref)
        discard;

    const int flags = mat.material.flags;
    bool detail_mask_reflection = (flags & SOSO_FLAG_DETAIL_MASK_REFLECTION) != 0;
    bool detail_mask_reflection_inv = (flags & SOSO_FLAG_DETAIL_MASK_REFLECTION_INV) != 0;
    bool detail_mask_change_color = (flags & SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR) != 0;
    bool detail_mask_change_color_inv = (flags & SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR_INV) != 0;
    bool detail_mask_self_illum = (flags & SOSO_FLAG_DETAIL_MASK_SELF_ILLUM) != 0;
    bool detail_mask_self_illum_inv = (flags & SOSO_FLAG_DETAIL_MASK_SELF_ILLUM_INV) != 0;
    bool detail_mask_multi_alpha = (flags & SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA) != 0;
    bool detail_mask_multi_alpha_inv = (flags & SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA_INV) != 0;

    vec4 primary_change_color = mat.material.input2;

    vec4 detail = get_color(detail_map_id, mat);
    bool has_detail = (mat.maps[detail_map_id].layer >> 24) != 0;
    vec4 multi = get_color(multi_map_id, mat);
    int multi_source = mat.maps[multi_map_id].layer >> 24;
    if(multi_source == 0)
        multi = vec4(0);
    float detail_factor = 1.0;
#ifdef MULTIPURPOSE_XBOX
    float specular_factor = multi.r;
    float illum_factor = multi.g;
    float color_change = multi.b;
#else
    // TODO: Figure out which one is really specular
    // Guerilla only lies, it says R is specular
    // but some important models don't get shiny that way
    // Some models have correct specular in R/B, others in A
    // Master Chief has A as specular
    // Old-style marines have specular in R or B
    float specular_factor = multi.a;
    float illum_factor = multi.g;
    float color_change = multi.a;
#endif

    if((render_flags & RENDER_FLAG_ONLY_DIFFUSE) != 0)
        return vec4(color.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_DETAIL) != 0)
        return vec4(detail.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE) != 0)
        return vec4(multi.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE2) != 0)
        return vec4(multi.a, color.a, detail.a, 1);
    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
        return vec4(frag.normal, 1);

    // TODO: Include animation on illum_factor

#if USE_REFLECTIONS == 1
    vec3 view_world = normalize(camera_position - frag.position);
    vec3  reflection  = vec3(0);
    float refl_amount = 0.0;
    if((render_flags & RENDER_FLAG_REFLECTION) != 0)
    {
        float NdotV_m      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
        float fresnel_m    = 1.0 - NdotV_m;
        vec3 reflect_dir   = reflect(-view_world, frag.normal);
        vec4 perp_m        = mat.material.input3;
        vec4 para_m        = mat.material.input4;
        /* Perpendicular at grazing, parallel head-on, as shader_environment
         * reads them. */
        float refl_strength = mix(para_m.a,   perp_m.a,   fresnel_m);
        vec3  refl_color    = mix(para_m.rgb, perp_m.rgb, fresnel_m);
        /* The cube map, tinted per shader. How much of it replaces the
         * surface is the shader's own reflection brightness times the
         * multipurpose map's shiny channel, so a dull ammo bag (brightness
         * 0.5) and a lacquered storage crate (1.0) stay apart, and a model's
         * unpolished parts keep their diffuse. */
        reflection  = get_cube_color(reflect_dir, mat).rgb * refl_color;
        refl_amount = clamp(refl_strength * specular_factor, 0.0, 1.0);
        if((render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0)
            return vec4(reflection * refl_amount, 1);
    }
    if((render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0)
        return vec4(0.0, 0.0, 0.0, 1.0); /* non-reflective surfaces -> black */
#else
    vec3  reflection  = vec3(0);
    float refl_amount = 0.0;
#endif

    // Diffuse lighting via geometry normal (soso has no bump map texture).
    // frag.normal is world-space and interpolated per-vertex (Gouraud approximation).
    /* Skybox lighting: every light whose flags cover where the camera stands
     * (.w holds them: 1 = exteriors, 2 = interiors), over the matching
     * ambient. Without the flag test a level's sun lights its corridors as
     * brightly as its hillsides. */
    bool interior   = (uint(mat.material.flags) & MATERIAL_FLAG_INTERIOR) != 0u;
    uint light_mask = interior ? 2u : 1u;
    vec3 lit = interior
        ? world.fog.indoor_ambient.rgb * world.fog.indoor_ambient.a
        : world.fog.outdoor_ambient.rgb * world.fog.outdoor_ambient.a;
    for(int li = 0; li < MAX_WORLD_LIGHTS; li++)
    {
        vec4 light_dir = world.lighting[li].light_direction;
        if(dot(light_dir.xyz, light_dir.xyz) < 1e-6)
            continue;
        if((uint(light_dir.w) & light_mask) == 0u)
            continue;
        lit += world.lighting[li].light_color.rgb
             * world.lighting[li].light_color.a
             * max(0.0, dot(frag.normal, normalize(light_dir.xyz)));
    }
    lit = clamp(lit, 0.1, 1.0);

    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
        return vec4(normalize(frag.normal), 1);

    if(detail_mask_reflection)
        detail_factor = specular_factor;
    else if(detail_mask_reflection_inv)
        detail_factor = 1 - specular_factor;
    else if(detail_mask_change_color)
        detail_factor = color_change;
    else if(detail_mask_change_color_inv)
        detail_factor = 1 - color_change;
    else if(detail_mask_self_illum)
        detail_factor = illum_factor;
    else if(detail_mask_self_illum_inv)
        detail_factor = 1 - illum_factor;
    else if(detail_mask_multi_alpha)
        detail_factor = multi.a;
    else if(detail_mask_multi_alpha_inv)
        detail_factor = 1 - multi.a;

    if(!has_detail)
        detail_factor = 0.0;

    color.rgb = color.rgb * mix(
        vec3(1), primary_change_color.rgb,
        color_change * primary_change_color.a);
    vec3 albedo = color.rgb;
    color.rgb   = clamp(lit * color.rgb + albedo * illum_factor, 0.0, 1.0);

    /* The detail map combines by the shader's own function, the default being
     * the same 2x modulate the environment shader uses; plain multiplication
     * halves the model. detail_factor stays the mask that says how much of the
     * detail applies. */
    uint soso_detail_func =
        (uint(flags) & SOSO_FLAG_DETAIL_FUNC_MUL) != 0u
            ? DETAIL_MULTIPLY
            : ((uint(flags) & SOSO_FLAG_DETAIL_FUNC_DOUBLE_BIAS_ADD) != 0u
                   ? DETAIL_BIASED_ADD
                   : DETAIL_BIASED_MULTIPLY);

    /* The reflection blends toward the cube, so the detail map has to land on
     * top of it or a shiny surface loses its panelling entirely — the hull of
     * a dropship is reflective across its whole multipurpose mask. On a matte
     * surface the reflection is zero and the order makes no difference. */
    color.rgb = mix(color.rgb, reflection, refl_amount * 0.8);
    color.rgb = clamp(
        mix(color.rgb, apply_detail(color.rgb, detail.rgb, soso_detail_func),
            detail_factor), 0.0, 1.0);
    return color;
}

vec4 shader_glass(in Material mat)
{
    vec4 diffuse    = get_color(base_map_id, mat);
    int  flags      = mat.material.flags;
    bool alpha_test = (flags & 0x1) != 0;

    vec3 bg_tint = mat.material.input2.rgb;
    vec4 perp    = mat.material.input3; // tint.rgb + brightness.a
    vec4 para    = mat.material.input4;

    vec3  view_world = normalize(camera_position - frag.position);
    float NdotV      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
    float fresnel    = 1.0 - NdotV;

    vec3 color = diffuse.rgb * bg_tint;

#if USE_REFLECTIONS == 1
    vec3  reflect_dir = reflect(-view_world, frag.normal);
    vec3  refl        = get_cube_color(reflect_dir, mat).rgb;
    vec3  refl_tint   = mix(para.rgb, perp.rgb, fresnel);
    float refl_str    = mix(para.a, perp.a, fresnel);
    color             = mix(color, refl * refl_tint, refl_str);
#endif

    return vec4(color, diffuse.a);
}

vec4 shader_meter(in Material mat)
{
    vec4  mask      = get_color(base_map_id, mat);
    float value     = mat.material.input1.x;
    float transp    = mat.material.input1.y;
    vec3  gmin      = mat.material.input2.rgb;
    float bg_transp = mat.material.input2.a;
    vec3  gmax      = mat.material.input3.rgb;
    vec3  background = mat.material.input4.rgb;
    vec3  tint      = mat.material.input5.rgb;

    bool  filled   = mask.r <= value;
    float t        = value > 0.0 ? mask.r / value : 0.0;
    vec3  gradient = mix(gmin, gmax, t) * tint;
    vec3  color    = filled ? gradient : background;
    float alpha    = filled ? 1.0 - transp : 1.0 - bg_transp;
    return vec4(color, alpha);
}

vec4 shader_plasma(in Material mat)
{
    const uint secondary_map_id = 1u;

    float intensity_exp = mat.material.input1.x;
    vec4  perp          = mat.material.input2; // tint.rgb + brightness.a
    vec4  para          = mat.material.input3;
    vec4  pdir          = mat.material.input4; // anim_dir.xyz + inv_period
    vec4  sdir          = mat.material.input5;

    // Two noise maps scroll in opposite directions; their interference is
    // the moving plasma pattern. anim_dir.xy * inv_period gives tiles/sec.
    vec2  p_off = pdir.xy * pdir.w * time;
    vec2  s_off = sdir.xy * sdir.w * time;
    vec4  primary   = get_color_with_offset(base_map_id, p_off, mat);
    vec4  secondary = get_color_with_offset(secondary_map_id, s_off, mat);

    vec3  view_world = normalize(camera_position - frag.position);
    float NdotV      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
    float fresnel    = 1.0 - NdotV;

    vec3  tint = mix(para.rgb, perp.rgb, fresnel);
    float str  = mix(para.a, perp.a, fresnel);

    float noise  = clamp(primary.r + secondary.r, 0.0, 1.0);
    float plasma = pow(noise, max(intensity_exp, 0.01));
    vec3  color  = tint * plasma * str;

    return vec4(color, plasma);
}

vec3 ripple_normal(in uint map_id, in vec2 offset, in Material mat)
{
    vec2 texel = exp2(mat.maps[map_id].bias)
               / vec2(textureSize(source_r8, 0).xy);
    vec2 step  = texel / max(
        mat.maps[map_id].uv_scale * mat.maps[map_id].atlas_scale, vec2(1e-6));
    step = max(step, abs(dFdx(frag.tex)) + abs(dFdy(frag.tex)));

    float here = get_color_with_offset(map_id, offset, mat).r;
    float du   = get_color_with_offset(map_id, offset + vec2(step.x, 0.0), mat).r;
    float dv   = get_color_with_offset(map_id, offset + vec2(0.0, step.y), mat).r;

    return normalize(vec3(here - du, here - dv, 1.0));
}

vec4 shader_water(in Material mat)
{
    const int ALPHA_MODULATES_REFLECT    = 0x1;
    const int COLOR_MODULATES_BACKGROUND = 0x2;
    const uint bump_map_id = 1u;

    int  flags         = mat.material.flags & 0x3;
    vec4 parallel      = mat.material.input2;
    vec4 perpendicular = mat.material.input3;
    vec4 base          = get_color(base_map_id, mat);

    // Up to four ripple layers, each with the angle, rate, offset and weight
    // its entry in the shader's ripple block states.
    // get_bump returns tangent-space normals decoded to [-1, 1].
    vec4 angles        = mat.material.input4;
    vec4 velocities    = mat.material.input5;
    vec4 contributions = mat.material.input6;
    vec2 offsets[4]    = vec2[4](
        mat.material.input7.xy, mat.material.input7.zw,
        mat.material.input8.xy, mat.material.input8.zw);

    vec3  bump_sum = vec3(0.0);
    float weight   = 0.0;
    for(int i = 0; i < 4; i++)
    {
        /* Every layer is sampled whether it contributes or not: skipping one
         * would make the texture gradient undefined for the whole quad. */
        vec2 dir = vec2(cos(angles[i]), sin(angles[i]));
        bump_sum += contributions[i] * ripple_normal(
            bump_map_id, offsets[i] + dir * velocities[i] * time, mat);
        weight   += contributions[i];
    }
    vec3 bump_ts = weight > 0.0 ? normalize(bump_sum) : vec3(0.0, 0.0, 1.0);

    // World-space quantities for reflection and Fresnel.
    // tbn_matrix() columns are T/B/N so it transforms tangent -> world.
    vec3 view_world   = normalize(camera_position - frag.position);
    vec3 world_normal = normalize(tbn_matrix() * bump_ts);

    /* Linear in the view angle, as every other class here blends its
     * perpendicular and parallel pair; an exponent here pins all but the most
     * grazing water to the perpendicular brightness and it turns see-through. */
    float NdotV   = clamp(dot(world_normal, view_world), 0.0, 1.0);
    float fresnel = 1.0 - NdotV;

    /* Perpendicular at grazing, parallel head-on, as shader_environment and
     * the other classes here read the pair. brightness (alpha channel) is the
     * view-angle reflection strength, not a color scale. */
    vec3  tint      = mix(parallel.rgb, perpendicular.rgb, fresnel);
    float out_alpha = mix(parallel.a,   perpendicular.a,   fresnel);

    vec3 reflect_color = vec3(0.0);
#if USE_REFLECTIONS == 1
    vec3 reflect_dir = reflect(-view_world, world_normal);
    reflect_color = tint * get_cube_color(reflect_dir, mat).rgb;
#endif

    if((flags & ALPHA_MODULATES_REFLECT) != 0)
        out_alpha *= base.a;

    if((flags & COLOR_MODULATES_BACKGROUND) != 0)
        reflect_color += base.rgb * out_alpha;

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
    Material material = mats.instance[frag.instanceId];

    uint material_id = get_material_id(material);
    vec4 color;

    if(material_id == MATERIAL_SENV)
    {
        color = shader_environment(material);
#if USE_MODEL_SHADERS == 1
    } else if(material_id == MATERIAL_SOSO)
    {
        color = shader_model(material);
#endif
    } else if(material_id == MATERIAL_SCHI)
    {
        final_color = shader_chicago(material);
        return;
    } else if(material_id == MATERIAL_SCEX)
    {
        final_color = shader_chicago_extended(material);
        return;
    } else if(material_id == MATERIAL_SWAT)
    {
        color = shader_water(material);
    } else if(material_id == MATERIAL_SGLA)
    {
        color = shader_glass(material);
    } else if(material_id == MATERIAL_SMET)
    {
        color = shader_meter(material);
    } else if(material_id == MATERIAL_SPLA)
    {
        color = shader_plasma(material);
    } else if(material_id == MATERIAL_SOTR)
    {
        final_color = shader_transparent(material);
        return;
    } else
        color = vec4(0, 1, 0, 1);

    if((render_flags & RENDER_FLAG_FOG) == 0)
    {
        final_color = color;
        return;
    }

    bool  fog_interior = (render_flags & RENDER_FLAG_INTERIOR) != 0;
    vec4  fog_props    = fog_interior
        ? world.fog.indoor_color : world.fog.outdoor_color;
    vec2  fog_range    = fog_interior
        ? world.fog.distances.xy : world.fog.distances.zw;
    vec4 fog_color = vec4(fog_props.xyz, 1);
    float fog_distance = length(frag.position - camera_position);
    fog_distance = (fog_distance - fog_range.x) / max(fog_range.y, 1.0);
    fog_distance = clamp(exp(-fog_distance * fog_props.w), 0, 1);
    final_color = mix(
        color,
        fog_color,
        1 - fog_distance);
}
