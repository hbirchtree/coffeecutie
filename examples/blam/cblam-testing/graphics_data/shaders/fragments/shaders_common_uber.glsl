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

const int RENDER_FLAG_FOG             = 0x1;
const int RENDER_FLAG_LIGHTMAP        = 0x2;
const int RENDER_FLAG_REFLECTION      = 0x4;
const int RENDER_FLAG_ONLY_REFLECTIONS = 0x80;
const int RENDER_FLAG_ONLY_NORMALS   = 0x10;
const int RENDER_FLAG_ONLY_NORMALMAP = 0x20;
const int RENDER_FLAG_ONLY_LIGHTMAP  = 0x40;
const int RENDER_FLAG_ONLY_MULTIPURPOSE = 0x100;
const int RENDER_FLAG_ONLY_MULTIPURPOSE2 = 0x200;
const int RENDER_FLAG_ONLY_DIFFUSE = 0x400;

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

#if USE_LIGHTMAPS == 1
/* Format-aware lightmap fetch: the lightmap's format is encoded in the high
 * byte of its layer (same scheme as get_color). Stock maps use RGB565
 * lightmaps; HD/Refined maps store them as RGBA8. Sample the matching bucket
 * instead of the old hardcoded RGB565-only `lightmaps` sampler. */
vec4 get_light(in uint instance, in vec2 light_tex)
{
    vec2 light_scale  = mats.instance[instance].lightmap.atlas_scale;
    vec2 light_offset = mats.instance[instance].lightmap.atlas_offset;
    int  tex_id       = mats.instance[instance].lightmap.layer;
    vec2 uv           = light_tex * light_scale + light_offset;
    int  layer        = tex_id & 0xFFFF;
    uint source       = uint(tex_id) >> 24;

    if(source == TEX_RGBA8)
        return texture(source_rgba8, vec3(uv, layer), -100.0).bgra;
    return texture(source_rgb565, vec3(uv, layer), -100.0);
}
#endif

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
#else
vec4 get_cube_color(in vec3 tex_coord)
{
    return vec4(1);
}
#endif

#if USE_NORMALMAP == 1
vec4 get_bump(in uint map_id, in vec2 offset)
{
    vec3 normal = normalize(get_color_with_offset(map_id, offset).rgb * 2.0 - 1.0);
    normal = /*tbn_matrix() **/ normal;
    return vec4(normal, dot(normal, light_direction()));
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
    vec4 lightmap = (render_flags & RENDER_FLAG_LIGHTMAP) != 0
        ? get_light(frag.instanceId, frag.light_tex)
        : vec4(1.0);
    if((render_flags & RENDER_FLAG_ONLY_LIGHTMAP) != 0)
        return lightmap;
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
    vec3 reflection   = vec3(1.0);
    vec3 refl_color   = vec3(1.0);
    float refl_strength = 0.0;
    if(reflective > 0 && (render_flags & RENDER_FLAG_REFLECTION) != 0)
    {
        uint reflect_type = uint((mats.instance[frag.instanceId].material.flags >> 7) & 0x3);
        float lightmap_brightness = mats.instance[frag.instanceId].material.input1.x;

        vec3 view_world  = normalize(camera_position - frag.position);
        vec3 world_bump  = normalize(tbn_matrix() * normal.rgb);
        vec3 surf_normal = normalize(frag.normal);

        // bumped_cube=0: perturbed normal; flat_cube=1: geometric normal; bumped_radiosity=2: bumped
        vec3 refl_normal = (reflect_type == 1u) ? surf_normal : world_bump;
        float NdotV      = clamp(dot(refl_normal, view_world), 0.0, 1.0);
        vec3 reflect_dir = reflect(-view_world, refl_normal);
        reflection       = get_cube_color(reflect_dir).rgb;

        vec4 perp_color     = mats.instance[frag.instanceId].material.input2;
        vec4 parallel_color = mats.instance[frag.instanceId].material.input3;
        float t   = 1.0 - NdotV; // perp=grazing(NdotV=0), parallel=straight-on(NdotV=1)
        refl_color    = mix(parallel_color.rgb, perp_color.rgb, t);
        refl_strength = mix(parallel_color.a,   perp_color.a,   t);

        // lightmap_brightness scales how much the lightmap modulates reflection strength
#if USE_LIGHTMAPS == 1
        float lm_luma   = dot(lightmap.rgb, vec3(0.299, 0.587, 0.114));
        refl_strength  *= mix(1.0, lm_luma, lightmap_brightness);
#endif
    }
#endif
#if USE_NORMALMAP == 1
    float bump_factor = clamp((normal.a + 1.0) / (light_direction().z + 1.0), 0.1, 2.0);
    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
        return vec4(normalize(frag.normal), 1);
    if((render_flags & RENDER_FLAG_ONLY_NORMALMAP) != 0)
        return vec4(normal.rgb, 1);
#endif
    vec3 out_color = base.rgb *
        micro.rgb *
        blend.rgb *
#if USE_LIGHTMAPS == 1
        lightmap.rgb *
#endif
#if USE_REFLECTIONS == 1
        mix(vec3(1), reflection * refl_color, refl_strength) *
#endif
#if USE_NORMALMAP == 1
        bump_factor *
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

vec4 chicago_blend(vec4 c1, vec4 c2, vec4 c3, vec4 c4, uint flags)
{
    vec4 out_color = vec4(1.0);
    chicago_stage(out_color, c1, c1, c2, flags & 0xFFu);
    if((uint(mats.instance[frag.instanceId].maps[2].layer) >> 24) != 0u)
        chicago_stage(out_color, out_color, c2, c3, (flags >> 8) & 0xFFu);
    if((uint(mats.instance[frag.instanceId].maps[3].layer) >> 24) != 0u)
        chicago_stage(out_color, out_color, c3, c4, (flags >> 16) & 0xFFu);
    return out_color;
}

vec4 shader_chicago()
{
    vec2 o1 = mats.instance[frag.instanceId].material.input1.xy;
    vec2 o2 = mats.instance[frag.instanceId].material.input2.xy;

    vec4 c1 = get_color_with_offset(0u, o1);
    vec4 c2 = get_color_with_offset(1u, o2);

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

/* Get color input (shader_transparent::input_t) as vec3 */
vec3 sotr_cin(uint i, vec4 m0, vec4 m1, vec4 m2, vec4 m3,
              vec4 sc0, vec4 sc1, vec4 c0, vec4 c1)
{
    if(i ==  1u) return vec3(1);
    if(i ==  2u) return vec3(0.5);
    if(i ==  3u) return vec3(-1);
    if(i ==  4u) return vec3(-0.5);
    if(i ==  5u) return m0.rgb;
    if(i ==  6u) return m1.rgb;
    if(i ==  7u) return m2.rgb;
    if(i ==  8u) return m3.rgb;
    if(i == 11u) return sc0.rgb;
    if(i == 12u) return sc1.rgb;
    if(i == 13u) return c0.rgb;   // constant_color0
    if(i == 14u) return c1.rgb;   // constant_color1
    if(i == 15u) return vec3(m0.a);
    if(i == 16u) return vec3(m1.a);
    if(i == 17u) return vec3(m2.a);
    if(i == 18u) return vec3(m3.a);
    if(i == 21u) return vec3(sc0.a);
    if(i == 22u) return vec3(sc1.a);
    if(i == 23u) return vec3(c0.a); // constant_alpha0
    if(i == 24u) return vec3(c1.a); // constant_alpha1
    return vec3(0); // zero (0) and unhandled vertex inputs
}

/* Get alpha input (blam::shader::color_input) */
float sotr_ain(uint i, vec4 m0, vec4 m1, vec4 m2, vec4 m3,
               float sa0, float sa1, float ca0, float ca1)
{
    if(i ==  1u) return 1.0;
    if(i ==  2u) return 0.5;
    if(i ==  3u) return -1.0;
    if(i ==  4u) return -0.5;
    if(i ==  5u) return m0.a;
    if(i ==  6u) return m1.a;
    if(i ==  7u) return m2.a;
    if(i ==  8u) return m3.a;
    if(i == 11u) return sa0;
    if(i == 12u) return sa1;
    if(i == 13u) return ca0;
    if(i == 14u) return ca1;
    if(i == 15u) return m0.b;
    if(i == 16u) return m1.b;
    if(i == 17u) return m2.b;
    if(i == 18u) return m3.b;
    if(i == 21u) return sa0;
    if(i == 22u) return sa1;
    if(i == 23u) return ca0;
    if(i == 24u) return ca1;
    return 0.0;
}

vec4 shader_transparent()
{
    vec4 m0 = get_color(0u);
    vec4 m1 = get_color(1u);
    vec4 m2 = get_color(2u);
    vec4 m3 = get_color(3u);

    int num_stages = int(tr.instance[frag.instanceId].num_stages);
    if(num_stages == 0)
        return m0;

    vec4 sc0    = vec4(0);
    vec4 sc_alt = vec4(0);

    for(int si = 0; si < num_stages && si < 4; si++)
    {
        TransparentStage s = tr.instance[frag.instanceId].stages[si];
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

        /* Decode outputs */
        uint c_ab_d  = outs & 0xFu,        c_ab_fn = (outs >> 4) & 1u;
        uint c_cd_d  = (outs >> 5) & 0xFu,  c_cd_fn = (outs >> 9) & 1u;
        uint c_sum_d = (outs >> 10) & 0xFu, c_om   = (outs >> 14) & 7u;
        uint a_ab_d  = (outs >> 16) & 0xFu;
        uint a_cd_d  = (outs >> 20) & 0xFu;
        uint a_sum_d = (outs >> 24) & 0xFu, a_om   = (outs >> 28) & 7u;

        vec4 c0 = s.color0;
        vec4 c1 = s.color1;

        /* Color: get + map inputs */
        vec3 ca = sotr_cmap(sotr_cin(ca_i, m0, m1, m2, m3, sc0, sc_alt, c0, c1), ca_m);
        vec3 cb = sotr_cmap(sotr_cin(cb_i, m0, m1, m2, m3, sc0, sc_alt, c0, c1), cb_m);
        vec3 cc = sotr_cmap(sotr_cin(cc_i, m0, m1, m2, m3, sc0, sc_alt, c0, c1), cc_m);
        vec3 cd = sotr_cmap(sotr_cin(cd_i, m0, m1, m2, m3, sc0, sc_alt, c0, c1), cd_m);

        vec3 c_ab = c_ab_fn == 0u ? ca * cb : vec3(dot(ca, cb));
        vec3 c_cd = c_cd_fn == 0u ? cc * cd : vec3(dot(cc, cd));

        /* Route color outputs (1=sc0, 2=sc_alt) */
        if(c_ab_d  == 1u) sc0.rgb = sotr_omap(c_ab, c_om);
        else if(c_ab_d  == 2u) sc_alt.rgb = sotr_omap(c_ab, c_om);
        if(c_cd_d  == 1u) sc0.rgb = sotr_omap(c_cd, c_om);
        else if(c_cd_d  == 2u) sc_alt.rgb = sotr_omap(c_cd, c_om);
        if(c_sum_d == 1u) sc0.rgb = sotr_omap(c_ab + c_cd, c_om);
        else if(c_sum_d == 2u) sc_alt.rgb = sotr_omap(c_ab + c_cd, c_om);

        /* Alpha: get + map inputs */
        float aa = sotr_smap(sotr_ain(aa_i, m0, m1, m2, m3, sc0.a, sc_alt.a, c0.a, c1.a), aa_m);
        float ab = sotr_smap(sotr_ain(ab_i, m0, m1, m2, m3, sc0.a, sc_alt.a, c0.a, c1.a), ab_m);
        float ac = sotr_smap(sotr_ain(ac_i, m0, m1, m2, m3, sc0.a, sc_alt.a, c0.a, c1.a), ac_m);
        float ad = sotr_smap(sotr_ain(ad_i, m0, m1, m2, m3, sc0.a, sc_alt.a, c0.a, c1.a), ad_m);

        float a_ab = aa * ab;
        float a_cd = ac * ad;

        if(a_ab_d  == 1u) sc0.a = sotr_somap(a_ab, a_om);
        else if(a_ab_d  == 2u) sc_alt.a = sotr_somap(a_ab, a_om);
        if(a_cd_d  == 1u) sc0.a = sotr_somap(a_cd, a_om);
        else if(a_cd_d  == 2u) sc_alt.a = sotr_somap(a_cd, a_om);
        if(a_sum_d == 1u) sc0.a = sotr_somap(a_ab + a_cd, a_om);
        else if(a_sum_d == 2u) sc_alt.a = sotr_somap(a_ab + a_cd, a_om);
    }


    /* blend_mode (chicago::framebuffer_blending):
     * 0=alpha_blend 1=multiply 2=double_multiply 3=add 4=subtract
     * 5=component_min 6=component_max 7=alpha_multiply_add
     * The engine's additive blend is (SRC_ALPHA, ONE), but Halo's true `add`
     * and component_max are alpha-independent. Force alpha=1 there so the
     * (SRC_ALPHA, ONE) state becomes straight additive (ONE, ONE). */
    uint bm = tr.instance[frag.instanceId].blend_mode;
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

vec4 shader_model()
{
    const uint multi_map_id  = 1u;
    const uint detail_map_id = 2u;

    float alpha_ref = 0.5;
    vec4 color = get_color(base_map_id);
    if(color.a < alpha_ref)
        discard;

    const int flags = mats.instance[frag.instanceId].material.flags;
    bool detail_mask_reflection = (flags & SOSO_FLAG_DETAIL_MASK_REFLECTION) != 0;
    bool detail_mask_reflection_inv = (flags & SOSO_FLAG_DETAIL_MASK_REFLECTION_INV) != 0;
    bool detail_mask_change_color = (flags & SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR) != 0;
    bool detail_mask_change_color_inv = (flags & SOSO_FLAG_DETAIL_MASK_CHANGE_COLOR_INV) != 0;
    bool detail_mask_self_illum = (flags & SOSO_FLAG_DETAIL_MASK_SELF_ILLUM) != 0;
    bool detail_mask_self_illum_inv = (flags & SOSO_FLAG_DETAIL_MASK_SELF_ILLUM_INV) != 0;
    bool detail_mask_multi_alpha = (flags & SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA) != 0;
    bool detail_mask_multi_alpha_inv = (flags & SOSO_FLAG_DETAIL_MASK_MULTI_ALPHA_INV) != 0;

    vec4 primary_change_color = mats.instance[frag.instanceId].material.input2;

    vec4 multi = get_color(multi_map_id);
    int multi_source = mats.instance[frag.instanceId].maps[multi_map_id].layer >> 24;
    if(multi_source == 0)
        multi = vec4(0);
    float detail_factor = 1.0;
#ifdef MULTIPURPOSE_XBOX
    float specular_factor = multi.r;
    float illum_factor = multi.g;
    float color_change = multi.b;
#else
    float specular_factor = multi.b;
    float illum_factor = multi.g;
    float color_change = multi.a;
#endif

    if((render_flags & RENDER_FLAG_ONLY_DIFFUSE) != 0)
        return vec4(color.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE) != 0)
        return vec4(multi.rgb, 1);
    if((render_flags & RENDER_FLAG_ONLY_MULTIPURPOSE2) != 0)
        return vec4(multi.a, color.a, 0, 1);

    // TODO: Include animation on illum_factor

#if USE_REFLECTIONS == 1
    vec3 view_world = normalize(camera_position - frag.position);
    vec3 reflection = vec3(1);
    // TODO: Find out why boulder_moss_large.shader_model becomes so glossy
    // TODO: Find out why visor is opaque, but flipping the mix()
    //       makes other shiny objects opaque
    if((render_flags & RENDER_FLAG_REFLECTION) != 0)
    {
        float NdotV_m      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
        float fresnel_m    = 1.0 - NdotV_m;
        vec3 reflect_dir   = reflect(-view_world, frag.normal);
        vec4 perp_m        = mats.instance[frag.instanceId].material.input3;
        vec4 para_m        = mats.instance[frag.instanceId].material.input4;
        // Brightness controls blend strength; tint colors the reflection.
        // Keeping them separate prevents zero-brightness from darkening the base.
        float refl_strength = mix(perp_m.a,   para_m.a,   fresnel_m);
        vec3  refl_color    = mix(perp_m.rgb, para_m.rgb, fresnel_m);
        reflection = mix(
            vec3(1),
            get_cube_color(reflect_dir).rgb + refl_color,
            specular_factor * NdotV_m);
        if((render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0)
            return vec4(get_cube_color(reflect_dir).rgb + refl_color, 1);
    }
    if((render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0)
        return vec4(0.0, 0.0, 0.0, 1.0); /* non-reflective surfaces -> black */
#else
    vec3 reflection = vec3(1);
#endif

    // Diffuse lighting via geometry normal (soso has no bump map texture).
    // frag.normal is world-space and interpolated per-vertex (Gouraud approximation).
    vec3 world_light = normalize(world.lighting[INTERIOR_LIGHTING].light_direction.xyz);
    float NdotL = max(0.1, dot(frag.normal, world_light));
    if(illum_factor > NdotL)
        NdotL = illum_factor;
    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
        return vec4(normalize(frag.normal), 1);

    vec4 detail = get_color(detail_map_id);
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
        detail_factor = 4;
    else if(detail_mask_multi_alpha_inv)
        detail_factor = 4;

    color.rgb = color.rgb * mix(
        vec3(1), primary_change_color.rgb,
        color_change * primary_change_color.a);
    color.rgb = clamp(NdotL * color.rgb, 0, 1);
    color.rgb = color.rgb * mix(vec3(1), detail.rgb, detail_factor) * reflection;
    return color;
}

vec4 shader_glass()
{
    vec4 diffuse    = get_color(base_map_id);
    int  flags      = mats.instance[frag.instanceId].material.flags;
    bool alpha_test = (flags & 0x1) != 0;

    vec3 bg_tint = mats.instance[frag.instanceId].material.input2.rgb;
    vec4 perp    = mats.instance[frag.instanceId].material.input3; // tint.rgb + brightness.a
    vec4 para    = mats.instance[frag.instanceId].material.input4;

    vec3  view_world = normalize(camera_position - frag.position);
    float NdotV      = clamp(dot(frag.normal, view_world), 0.0, 1.0);
    float fresnel    = 1.0 - NdotV;

    vec3 color = diffuse.rgb * bg_tint;

#if USE_REFLECTIONS == 1
    vec3  reflect_dir = reflect(-view_world, frag.normal);
    vec3  refl        = get_cube_color(reflect_dir).rgb;
    vec3  refl_tint   = mix(para.rgb, perp.rgb, fresnel);
    float refl_str    = mix(para.a, perp.a, fresnel);
    color             = mix(color, refl * refl_tint, refl_str);
#endif

    return vec4(color, alpha_test ? diffuse.a : 1.0);
}

vec4 shader_meter()
{
    vec4  mask      = get_color(base_map_id);
    float value     = mats.instance[frag.instanceId].material.input1.x;
    float transp    = mats.instance[frag.instanceId].material.input1.y;
    vec3  gmin      = mats.instance[frag.instanceId].material.input2.rgb;
    float bg_transp = mats.instance[frag.instanceId].material.input2.a;
    vec3  gmax      = mats.instance[frag.instanceId].material.input3.rgb;
    vec3  background = mats.instance[frag.instanceId].material.input4.rgb;
    vec3  tint      = mats.instance[frag.instanceId].material.input5.rgb;

    bool  filled   = mask.r <= value;
    float t        = value > 0.0 ? mask.r / value : 0.0;
    vec3  gradient = mix(gmin, gmax, t) * tint;
    vec3  color    = filled ? gradient : background;
    float alpha    = filled ? 1.0 - transp : 1.0 - bg_transp;
    return vec4(color, alpha);
}

vec4 shader_plasma()
{
    const uint secondary_map_id = 1u;
    vec4  primary   = get_color(base_map_id);
    vec4  secondary = get_color(secondary_map_id);

    float intensity_exp = mats.instance[frag.instanceId].material.input1.x;
    vec4  perp          = mats.instance[frag.instanceId].material.input2; // tint.rgb + brightness.a
    vec4  para          = mats.instance[frag.instanceId].material.input3;
    vec4  pdir          = mats.instance[frag.instanceId].material.input4; // anim_dir.xyz + inv_period
    vec4  sdir          = mats.instance[frag.instanceId].material.input5;

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
    } else if(material_id == MATERIAL_SPLA)
    {
        color = shader_plasma();
    } else if(material_id == MATERIAL_SOTR)
    {
        final_color = shader_transparent();
        return;
    } else
        color = vec4(0, 1, 0, 1);

    if((render_flags & RENDER_FLAG_FOG) == 0)
    {
        final_color = color;
        return;
    }

    vec4 fog_color = vec4(world.fog.outdoor_color.xyz, 1);
    float fog_distance = length(frag.position - camera_position);
    fog_distance = (fog_distance - world.fog.distances.z) / world.fog.distances.w;
    fog_distance = clamp(exp(-fog_distance * world.fog.outdoor_color.w), 0, 1);
    final_color = mix(
        color,
        fog_color,
        1 - fog_distance);
}
