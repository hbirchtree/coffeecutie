/* Halo 3 BSP/model fragment body, after h2.vert. Variants set:
 *   H3_DETAIL  0 none, 1 base x detail, 2 base x detail x detail2,
 *              3 base x detail/detail2 mixed by the base alpha
 *   H3_TERRAIN blend_map weights up to four base x detail layers
 * Shading is linear as in Halo 3: every texture is decoded by its bitmap's
 * curve (an exponent, 1 for linear), each detail layer is multiplied by
 * 2^2.2, and the result is encoded to gamma at the end. No lighting yet: a
 * sun term. */

layout(location=0) in vec3 f_world_pos;
layout(location=1) in vec2 f_texcoord;
layout(location=2) in vec3 f_normal;
layout(location=3) in vec2 f_detail_uv;
layout(location=4) in vec2 f_detail2_uv;
layout(location=6) in vec3 f_lightmap;
layout(location=9) in vec2 f_bump_uv;

layout(location=3) uniform int alpha_test;
/* H3's own uniforms start at 64, clear of h2.vert and h2_common */
layout(location=64) uniform vec4 albedo_tint;
layout(location=65) uniform int blended;
/* Decode exponents: base, detail, detail2 (terrain: per layer, below) */
layout(location=66) uniform vec3 curves;

#ifndef H3_DETAIL
#define H3_DETAIL 0
#endif
#ifndef H3_TERRAIN
#define H3_TERRAIN 0
#endif

#if H3_TERRAIN
layout(location=67, binding=0) uniform sampler2D base_m0;
layout(location=68, binding=1) uniform sampler2D base_m1;
layout(location=69, binding=2) uniform sampler2D base_m2;
layout(location=70, binding=3) uniform sampler2D base_m3;
layout(location=71, binding=4) uniform sampler2D detail_m0;
layout(location=72, binding=5) uniform sampler2D detail_m1;
layout(location=73, binding=6) uniform sampler2D detail_m2;
layout(location=74, binding=7) uniform sampler2D detail_m3;
layout(location=75, binding=8) uniform sampler2D blend_map;
/* Per layer: base uv scale (xy), detail uv scale (zw) */
layout(location=76) uniform vec4 layer_scale[4];
layout(location=80) uniform vec2 blend_scale;
/* Which layers the shader turns on, and which keep their detail */
layout(location=81) uniform vec4 layer_enabled;
layout(location=82) uniform vec4 layer_detailed;
/* Decode exponents per layer: base, detail */
layout(location=83) uniform vec4 layer_base_curve;
layout(location=84) uniform vec4 layer_detail_curve;
#else
layout(location=2, binding=0) uniform sampler2D diffuse;
#if H3_DETAIL >= 1
layout(location=9, binding=1) uniform sampler2D detail;
#endif
#if H3_DETAIL >= 2
layout(location=10, binding=2) uniform sampler2D detail2;
#endif
layout(location=85, binding=3) uniform sampler2D bump_map;
layout(location=86, binding=4) uniform sampler2D bump_detail_map;
/* 0 none, 1 bump map, 2 plus the detail bump map at bump uv * ratio */
layout(location=87) uniform int bump_mode;
layout(location=88) uniform vec2 bump_detail_ratio;
#endif

/* Baked light: SH coefficient pairs in layers 0-7, max luminance of each */
layout(location=89, binding=9) uniform sampler2DArray lightprobe;
layout(location=90) uniform vec4 lightprobe_scale;
layout(location=91) uniform float exposure;

layout(location=0) out vec4 out_color;

const float detail_multiplier = 4.59479; /* 2^2.2 */

vec3 decode(vec3 c, float curve)
{
    return pow(max(c, vec3(0.0)), vec3(curve));
}

vec3 surface_normal()
{
    return length(f_normal) > 0.5
        ? normalize(f_normal)
        : normalize(cross(dFdx(f_world_pos), dFdy(f_world_pos)));
}

#if !H3_TERRAIN
/* Tangent-space normal (x along +u, y along +v) in the frame of the uv
 * gradients */
vec3 bumped_normal(vec3 n)
{
    vec2 b = texture(bump_map, f_bump_uv).rg * 2.0 - 1.0;
    if(bump_mode == 2)
        b += texture(bump_detail_map, f_bump_uv * bump_detail_ratio).rg * 2.0 - 1.0;
    vec3 tn = normalize(vec3(b, sqrt(max(1.0 - dot(b, b), 0.0)) + 1e-3));
    vec3 dp1 = dFdx(f_world_pos), dp2 = dFdy(f_world_pos);
    vec2 duv1 = dFdx(f_bump_uv), duv2 = dFdy(f_bump_uv);
    vec3 dp2perp = cross(dp2, n), dp1perp = cross(n, dp1);
    vec3 t = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 bt = dp2perp * duv1.y + dp1perp * duv2.y;
    float s = inversesqrt(max(max(dot(t, t), dot(bt, bt)), 1e-20));
    return normalize(mat3(t * s, bt * s, n) * tn);
}
#endif

/* One linear SH coefficient (RGB) from its two DXT5 slices: length from
 * alpha, direction from rgb */
vec3 lightprobe_coefficient(float slice, float scale)
{
    vec4 a = texture(lightprobe, vec3(f_lightmap.xy, slice));
    vec4 b = texture(lightprobe, vec3(f_lightmap.xy, slice + 1.0));
    return ((a.rgb + b.rgb) * 2.0 - 2.0) * (a.a * b.a * scale);
}

/* Diffuse reflectance (irradiance / pi) of linear SH in D3DX order:
 * L0, then -y, z, -x */
vec3 baked_diffuse(vec3 n)
{
    vec3 l0 = lightprobe_coefficient(0.0, lightprobe_scale.x);
    vec3 l1 = lightprobe_coefficient(2.0, lightprobe_scale.y);
    vec3 l2 = lightprobe_coefficient(4.0, lightprobe_scale.z);
    vec3 l3 = lightprobe_coefficient(6.0, lightprobe_scale.w);
    vec3 e = 0.886227 * l0 + 1.023328 * (-n.y * l1 + n.z * l2 - n.x * l3);
    return max(e, vec3(0.0)) / 3.14159265;
}

#if H3_TERRAIN
vec3 terrain_layer(
    sampler2D base, sampler2D det, vec4 scale, float detailed, float base_curve, float detail_curve)
{
    vec3 c = decode(texture(base, f_texcoord * scale.xy).rgb, base_curve);
    vec3 d = decode(texture(det, f_texcoord * scale.zw).rgb, detail_curve) * detail_multiplier;
    return c * mix(vec3(1.0), d, detailed);
}
#endif

void main()
{
#if H3_TERRAIN
    vec4 w = texture(blend_map, f_texcoord * blend_scale) * layer_enabled;
    float total = w.x + w.y + w.z + w.w;
    w = total > 1e-4 ? w / total : vec4(1.0, 0.0, 0.0, 0.0);
    vec3 color = vec3(0.0);
    if(w.x > 0.0)
        color += w.x * terrain_layer(
            base_m0, detail_m0, layer_scale[0], layer_detailed.x,
            layer_base_curve.x, layer_detail_curve.x);
    if(w.y > 0.0)
        color += w.y * terrain_layer(
            base_m1, detail_m1, layer_scale[1], layer_detailed.y,
            layer_base_curve.y, layer_detail_curve.y);
    if(w.z > 0.0)
        color += w.z * terrain_layer(
            base_m2, detail_m2, layer_scale[2], layer_detailed.z,
            layer_base_curve.z, layer_detail_curve.z);
    if(w.w > 0.0)
        color += w.w * terrain_layer(
            base_m3, detail_m3, layer_scale[3], layer_detailed.w,
            layer_base_curve.w, layer_detail_curve.w);
    float alpha = 1.0;
#else
    vec4 base = texture(diffuse, f_texcoord);
    if(alpha_test != 0 && base.a < 0.5)
        discard;
    vec3 color = decode(base.rgb, curves.x);
#if H3_DETAIL >= 1
    vec3 d1 = decode(texture(detail, f_detail_uv).rgb, curves.y);
#endif
#if H3_DETAIL >= 2
    vec3 d2 = decode(texture(detail2, f_detail2_uv).rgb, curves.z);
#endif
#if H3_DETAIL == 1
    color *= d1 * detail_multiplier;
#elif H3_DETAIL == 2
    color *= d1 * d2 * detail_multiplier * detail_multiplier;
#elif H3_DETAIL == 3
    color *= mix(d1, d2, base.a) * detail_multiplier;
#endif
    color *= albedo_tint.rgb;
    float alpha = blended != 0 ? base.a * albedo_tint.a : 1.0;
#endif

    vec3 normal = surface_normal();
#if !H3_TERRAIN
    if(bump_mode != 0)
        normal = bumped_normal(normal);
#endif
    if(f_lightmap.z >= 0.0)
        color *= baked_diffuse(normal) * exposure;
    else
    {
        vec3 sun = normalize(vec3(0.3, 0.2, 1.0));
        color *= 0.45 + 0.55 * abs(dot(normal, sun));
    }
    out_color = vec4(pow(max(color, vec3(0.0)), vec3(1.0 / 2.2)), alpha);
}
