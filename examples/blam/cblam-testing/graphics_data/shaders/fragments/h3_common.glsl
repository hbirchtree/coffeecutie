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

layout(location=3) uniform int alpha_test; /* 1 base map alpha, 2 alpha_test_map */
/* H3's own uniforms start at 64, clear of h2.vert and h2_common */
layout(location=64) uniform vec4 albedo_tint;
layout(location=65) uniform int blended; /* 1 alpha blended, 2 additive */
/* RenderingParameters debug views: 1 albedo, 2 normals, 3 normal maps, 4
 * baked light, 5 reflections */
layout(location=149) uniform int debug_view;
/* ... and what they turn off: 1 fog, 2 reflections, 4 baked light */
layout(location=150) uniform int debug_off;
/* simple_lights.fx: per light position and size, direction and spherical
 * share, colour and smoothness, falloff scale and offset, bounding radius² */
layout(location=151) uniform vec4 simple_lights[40];
layout(location=191) uniform int simple_light_count;
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
/* alpha_test simple (2): its own map, at base uv * ratio */
layout(location=147, binding=5) uniform sampler2D alpha_test_map;
layout(location=148) uniform vec2 alpha_test_ratio;
#if H3_DETAIL >= 1
layout(location=9, binding=1) uniform sampler2D detail;
#endif
#if H3_DETAIL >= 2
layout(location=10, binding=2) uniform sampler2D detail2;
#endif
#if H3_DETAIL >= 4
/* detail_map_overlay (4) or detail_map3 (5), at base uv * ratio */
layout(location=121, binding=14) uniform sampler2D detail3;
layout(location=122) uniform vec2 detail3_ratio;
layout(location=123) uniform float detail3_curve;
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
layout(location=92) uniform float intensity_scale;
#include "h3_atmosphere.glsl"
layout(location=8) in vec4 f_vertex_light;

#if !H3_TERRAIN
/* Material model: 0 diffuse only, 1 two_lobe_phong, 2 cook_torrance; the
 * parameters as SpecularParams in h3_scene.h */
layout(location=93) uniform int material_model;
layout(location=94) uniform int specular_mask_mode;
layout(location=95) uniform vec4 spec_coefficients;
layout(location=96) uniform vec4 spec_lobe;
layout(location=97) uniform vec4 spec_tint0;
layout(location=98) uniform vec4 spec_tint1;
layout(location=100, binding=10) uniform sampler2D specular_mask_map;
layout(location=101, binding=11) uniform sampler2D material_map;
/* Self-illumination: 0 off, 1 simple, 2 with detail, 3 three channel */
layout(location=5) in vec2 f_illum_uv;
layout(location=104) uniform int self_illum_mode;
layout(location=120) uniform int black_point;
/* environment_mapping: 0 none, 1 dynamic (env_dynamic, RGBM x256), 2
 * per_pixel (env_map); env_tint rgb, contribution a */
layout(location=124) uniform int env_mode;
layout(location=125) uniform vec4 env_tint;
layout(location=126, binding=15) uniform samplerCube env_dynamic;
layout(location=127, binding=16) uniform samplerCube env_map;
layout(location=128) uniform float env_curve; /* per_pixel cube's decode exponent */
layout(location=105) uniform vec4 illum_color; /* rgb, intensity in a */
layout(location=106) uniform vec4 illum_channels[3];
layout(location=109) uniform vec2 illum_detail_ratio;
layout(location=110) uniform float illum_curve;
/* multilayer: depth, contrast, layer count, darkening per layer */
layout(location=145) uniform vec4 illum_layers;
/* multilayer: the map's uv scale, then the texcoord aspect ratio */
layout(location=146) uniform vec4 illum_layer_xform;
layout(location=111, binding=12) uniform sampler2D illum_map;
layout(location=112, binding=13) uniform sampler2D illum_detail_map;
#endif

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

struct baked_light_t
{
    vec3 l0, l1, l2, l3; /* Linear SH, D3DX order */
    vec3 dir;            /* Dominant light, towards it */
    vec3 intensity;
};

baked_light_t sample_baked_light()
{
    baked_light_t light;
    light.l0 = lightprobe_coefficient(0.0, lightprobe_scale.x);
    light.l1 = lightprobe_coefficient(2.0, lightprobe_scale.y);
    light.l2 = lightprobe_coefficient(4.0, lightprobe_scale.z);
    light.l3 = lightprobe_coefficient(6.0, lightprobe_scale.w);
    light.intensity = lightprobe_coefficient(8.0, intensity_scale);
    const vec3 lum = vec3(0.212656, 0.715158, 0.0721856);
    vec3 dir = vec3(-dot(light.l3, lum), -dot(light.l1, lum), dot(light.l2, lum));
    light.dir = dot(dir, dir) > 1e-12 ? normalize(dir) : vec3(0.0, 0.0, 1.0);
    return light;
}

/* SH linear terms dotted with a direction, per channel, as Halo 3 packs
 * them: (l3, l1, -l2) */
vec3 sh_linear(baked_light_t light, vec3 d)
{
    return vec3(
        dot(d, vec3(light.l3.r, light.l1.r, -light.l2.r)),
        dot(d, vec3(light.l3.g, light.l1.g, -light.l2.g)),
        dot(d, vec3(light.l3.b, light.l1.b, -light.l2.b)));
}

/* Diffuse radiance from linear SH in D3DX order (L0, then -y, z, -x) and
 * the dominant light, as Halo 3's ravi_order_2_with_dominant_light: the
 * dominant light leaves the SH and comes back as a clamped cosine */
vec3 baked_diffuse(baked_light_t light, vec3 n)
{
    vec3 dc = light.l0 - 0.2820948 * light.intensity;
    vec3 x1 = sh_linear(light, n) + 0.4886025 * dot(n, light.dir) * light.intensity;
    vec3 e = (0.886227 * dc - 1.023328 * x1) / 3.14159265;
    return max(e, vec3(0.0)) + max(dot(light.dir, n), 0.0) * light.intensity * 0.281;
}

/* calc_simple_lights_analytical's diffuse part */
vec3 dynamic_diffuse(vec3 n)
{
    vec3 sum = vec3(0.0);
    for(int i = 0; i < simple_light_count; i++)
    {
        vec4 position = simple_lights[i * 5];
        vec4 direction = simple_lights[i * 5 + 1];
        vec4 color = simple_lights[i * 5 + 2];
        vec4 falloff = simple_lights[i * 5 + 3];
        vec3  to = position.xyz - f_world_pos;
        float d2 = dot(to, to);
        if(d2 >= simple_lights[i * 5 + 4].x)
            continue;
        to *= inversesqrt(max(d2, 1e-8));
        vec2 f = vec2(1.0 / (position.w + d2), dot(to, direction.xyz));
        f = max(vec2(0.0001), f * falloff.xy + falloff.zw);
        f.y = pow(f.y, color.w) + direction.w;
        sum += color.rgb * clamp(f.x, 0.0, 1.0) * clamp(f.y, 0.0, 1.0) * max(0.05, dot(n, to));
    }
    return sum;
}

#if !H3_TERRAIN
/* Area specular from the SH around the reflection, order 2 (Halo 3's
 * calculate_area_specular_phong_order_2, red DC and all) */
vec3 area_specular(baked_light_t light, vec3 r, vec3 tint)
{
    return (light.l0.r * 0.4231425 - 0.3805236 * sh_linear(light, r)) * tint;
}

/* Specular radiance of the material model, and the diffuse scale; Halo 3's
 * calc_material_*_ps without dynamic lights, anti-shadow or env maps */
vec3 material_specular(
    baked_light_t light, vec3 n, vec3 albedo, float mask, out float diffuse_scale)
{
    diffuse_scale = spec_coefficients.y;
    if(material_model == 0)
        return vec3(0.0);
    vec3  v      = normalize(camera_position - f_world_pos);
    vec3  r      = 2.0 * dot(v, n) * n - v;
    float coeff  = spec_coefficients.x;
    float env_contribution = env_tint.a;
    vec3  analytic, area;
    if(material_model == 1)
    {
        float n_dot_v = max(dot(n, v), 0.0);
        float blend = pow(1.0 - n_dot_v, spec_lobe.z);
        float power = mix(spec_lobe.x, spec_lobe.y, blend);
        vec3  tint = mix(mix(spec_tint0.rgb, spec_tint1.rgb, blend), albedo, spec_lobe.w);
        float l_dot_r = dot(light.dir, r);
        analytic = l_dot_r > 0.0
            ? pow(l_dot_r, power) * ((power + 1.0) / 6.2832) * tint * light.intensity
            : vec3(0.0);
        area = area_specular(light, r, tint);
    } else
    {
        vec4 params = vec4(coeff, spec_lobe.y, env_tint.a, spec_lobe.x);
        if(spec_lobe.z > 0.5)
            params = texture(material_map, f_texcoord);
        coeff = params.r;
        env_contribution = params.b;
        if(spec_lobe.z > 0.5)
            diffuse_scale = 1.0 - params.r;
        vec3 reflectance = mix(spec_tint0.rgb, albedo, params.g);
        vec3 l = light.dir;
        float n_dot_l = dot(n, l), n_dot_v = dot(n, v);
        analytic = vec3(0.0);
        if(min(n_dot_l, n_dot_v) > 0.0)
        {
            vec3  h = normalize(v + l);
            float n_dot_h = dot(n, h), v_dot_h = dot(v, h);
            float g_term = 2.0 * n_dot_h * min(n_dot_l, n_dot_v) / (clamp(v_dot_h, 0.0, 1.0) + 1e-5);
            vec3  f0 = min(reflectance, vec3(0.999));
            vec3  sq = sqrt(f0);
            vec3  ior = (1.0 + sq) / (1.0 - sq);
            vec3  g = sqrt(ior * ior + v_dot_h * v_dot_h - 1.0);
            vec3  gpc = g + v_dot_h, gmc = g - v_dot_h;
            vec3  rr = (v_dot_h * gpc - 1.0) / (v_dot_h * gmc + 1.0);
            vec3  fresnel = 0.5 * (gmc * gmc) / (gpc * gpc + 1e-5) * (1.0 + rr * rr);
            float m2 = max(params.a, 0.05);
            m2 *= m2;
            float c2 = n_dot_h * n_dot_h;
            float distribution = exp((c2 - 1.0) / (m2 * c2)) / (m2 * c2 * c2 + 1e-5);
            analytic = distribution * clamp(g_term, 0.0, 1.0) / (3.14159265 * n_dot_v + 1e-5) * fresnel;
            analytic = min(analytic, vec3(n_dot_l + 1.0)) * light.intensity;
        }
        /* Stands in for the pre-integrated area lookup tables */
        area = area_specular(light, r, reflectance);
        analytic *= spec_tint1.rgb;
        area *= spec_tint1.rgb;
    }
    /* calc_environment_map_*_ps: the reflection weighted by the area
     * specular, so it follows the local light */
    vec3 env = vec3(0.0);
    if(env_mode != 0 && (debug_off & 2) == 0)
    {
        vec3 dir = vec3(r.x, -r.y, r.z);
        vec3 reflection;
        if(env_mode == 1)
        {
            vec4 c     = texture(env_dynamic, dir);
            reflection = c.rgb * c.a * 256.0;
        } else
        {
            vec4 c     = texture(env_map, dir);
            reflection = pow(max(c.rgb, vec3(0.0)), vec3(env_curve)) * c.a;
        }
        env = reflection * (env_contribution * mask * coeff) * env_tint.rgb * max(area, vec3(0.0));
    }
    return mask * coeff *
               (max(analytic, vec3(0.0)) * spec_coefficients.z + max(area * spec_coefficients.w, vec3(0.0))) +
           env;
}
#endif

#if !H3_TERRAIN
/* Halo 3's calc_self_illumination_*_ps; added after exposure, as the
 * game's ILLUM_SCALE keeps it independent of exposure */
vec3 self_illumination()
{
    if(self_illum_mode == 0)
        return vec3(0.0);
    if(self_illum_mode == 4)
    {
        /* multilayer: the map stacked into the surface along the view in
         * tangent space, each layer darker, the average raised to a contrast */
        vec3  n    = surface_normal();
        vec3  dp1  = dFdx(f_world_pos), dp2 = dFdy(f_world_pos);
        vec2  duv1 = dFdx(f_illum_uv), duv2 = dFdy(f_illum_uv);
        vec3  dp2perp = cross(dp2, n), dp1perp = cross(n, dp1);
        vec3  t    = dp2perp * duv1.x + dp1perp * duv2.x;
        vec3  bt   = dp2perp * duv1.y + dp1perp * duv2.y;
        float s    = inversesqrt(max(max(dot(t, t), dot(bt, bt)), 1e-20));
        vec3  v    = normalize(camera_position - f_world_pos);
        vec2  view = vec2(dot(v, t * s), dot(v, bt * s));
        float layers = max(illum_layers.z, 1.0);
        vec2  offset = view * illum_layer_xform.xy * vec2(illum_layer_xform.z, 1.0) * illum_layers.x / layers;
        vec2  uv     = f_illum_uv;
        float darken = 1.0;
        vec3  accum  = vec3(0.0);
        for(int i = 0; i < int(layers); i++)
        {
            accum += darken * decode(texture(illum_map, uv).rgb, illum_curve);
            uv -= offset;
            darken *= illum_layers.w;
        }
        return pow(accum / layers, vec3(illum_layers.y)) * illum_color.rgb * illum_color.a;
    }
    vec3 map = decode(texture(illum_map, f_illum_uv).rgb, illum_curve);
    if(self_illum_mode == 3)
        return (map.r * illum_channels[0].a * illum_channels[0].rgb +
                map.g * illum_channels[1].a * illum_channels[1].rgb +
                map.b * illum_channels[2].a * illum_channels[2].rgb) *
               illum_color.a;
    if(self_illum_mode == 2)
        map *= decode(texture(illum_detail_map, f_illum_uv * illum_detail_ratio).rgb, illum_curve) *
               detail_multiplier;
    return map * illum_color.rgb * illum_color.a;
}
#endif



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
    float tested = alpha_test == 2 ? texture(alpha_test_map, f_texcoord * alpha_test_ratio).a : base.a;
    if(alpha_test != 0 && tested < 0.5)
        discard;
    vec3 color = decode(base.rgb, curves.x);
#if H3_DETAIL >= 1
    vec4 detail_texel = texture(detail, f_detail_uv);
    vec3 d1 = decode(detail_texel.rgb, curves.y);
#endif
#if H3_DETAIL >= 2
    vec4 detail2_texel = texture(detail2, f_detail2_uv);
    vec3 d2 = decode(detail2_texel.rgb, curves.z);
#endif
#if H3_DETAIL == 1
    color *= d1 * detail_multiplier;
#elif H3_DETAIL == 2
    color *= d1 * d2 * detail_multiplier * detail_multiplier;
#elif H3_DETAIL == 3
    color *= mix(d1, d2, base.a) * detail_multiplier;
#elif H3_DETAIL >= 4
    vec4 detail3_texel = texture(detail3, f_texcoord * detail3_ratio);
    vec3 d3 = decode(detail3_texel.rgb, detail3_curve);
#endif
#if H3_DETAIL == 4
    /* two_detail_overlay: base x blend of the details by base alpha x overlay */
    color *= mix(d1, d2, base.a) * d3 * detail_multiplier * detail_multiplier;
#elif H3_DETAIL == 5
    /* three_detail_blend: base alpha walks from detail 1 to 2 to 3 */
    float blend1 = clamp(2.0 * base.a, 0.0, 1.0);
    float blend2 = clamp(2.0 * base.a - 1.0, 0.0, 1.0);
    color *= mix(mix(d1, d2, blend1), d3, blend2) * detail_multiplier;
#endif
    color *= albedo_tint.rgb;
    float coverage = base.a;
#if H3_DETAIL == 4
    coverage = mix(detail_texel.a, detail2_texel.a, base.a) * detail3_texel.a;
#elif H3_DETAIL == 5
    coverage = mix(mix(detail_texel.a, detail2_texel.a, blend1), detail3_texel.a, blend2);
#endif
#if H3_DETAIL == 2
    /* two_detail_black_point: the base alpha is a black point for the
     * details' alpha (apply_black_point in Halo 3's utilities.fx) */
    if(black_point != 0)
    {
        float mid = (base.a + 1.0) * 0.5;
        float a   = detail_texel.a * detail2_texel.a;
        coverage  = mid * clamp((a - base.a) / (mid - base.a), 0.0, 1.0) + clamp(a - mid, 0.0, 1.0);
    }
#endif
    float alpha = blended != 0 ? coverage * albedo_tint.a : 1.0;
#endif

    vec3 normal = surface_normal();
    vec3 geometric = normal;
#if !H3_TERRAIN
    if(bump_mode != 0)
        normal = bumped_normal(normal);
#endif
    /* Raised to 2.2 so they read as they are after the composite's gamma */
    if(debug_view == 1 || debug_view == 2 || debug_view == 3)
    {
        vec3 shown = debug_view == 1 ? color : (debug_view == 2 ? geometric : normal) * 0.5 + 0.5;
        out_color = vec4(debug_view == 1 ? shown : pow(shown, vec3(2.2)), alpha);
        return;
    }
    if(debug_view == 4)
        color = vec3(1.0);
    bool baked = (debug_off & 4) == 0;
    if(f_lightmap.z >= 0.0 && baked)
    {
        baked_light_t light = sample_baked_light();
#if H3_TERRAIN
        color *= (baked_diffuse(light, normal) + dynamic_diffuse(normal)) * exposure *
                 (debug_view == 5 ? 0.0 : 1.0);
#else
        float mask = specular_mask_mode == 1 ? base.a
                   : specular_mask_mode == 2 ? texture(specular_mask_map, f_texcoord).a
                                             : 1.0;
        float diffuse_scale;
        vec3  specular = material_specular(light, normal, color, mask, diffuse_scale);
        if(debug_view == 4)
            specular = vec3(0.0), diffuse_scale = 1.0;
        if(debug_view == 5)
            color = vec3(0.0);
        color = (color * (baked_diffuse(light, normal) + dynamic_diffuse(normal)) * diffuse_scale + specular) *
                exposure;
#endif
    }
    else if(f_vertex_light.a > 0.0 && baked)
        color *= (f_vertex_light.rgb * f_vertex_light.rgb * 4.0 + dynamic_diffuse(normal)) * exposure;
    else
    {
        vec3 sun = normalize(vec3(0.3, 0.2, 1.0));
        color *= 0.45 + 0.55 * abs(dot(normal, sun)) + dynamic_diffuse(normal) * exposure;
    }
#if !H3_TERRAIN
    /* material_model none (halograms): self-illumination alone */
    if(material_model == 3)
        color = vec3(0.0);
    if(debug_view == 0)
        color += self_illumination();
#endif
    vec3 extinction = vec3(1.0), inscatter = vec3(0.0);
    if((debug_off & 1) == 0)
        compute_scattering(camera_position, f_world_pos, extinction, inscatter);
    /* What is added to the scene only fades; the haze in front is already there */
    color = color * extinction + (blended == 2 ? vec3(0.0) : inscatter * exposure);
    /* Linear HDR: the composite applies Halo 3's tone curve */
    out_color = vec4(max(color, vec3(0.0)), alpha);
}
