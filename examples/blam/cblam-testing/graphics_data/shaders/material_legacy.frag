#version 100
#ifdef GL_OES_standard_derivatives
#extension GL_OES_standard_derivatives : enable
#endif
precision mediump float;

/* Enhanced ES2 materials, the counterpart of shaders_common_uber.glsl. One
 * texture per map, its format and the material's switches as constants
 * (see material_legacy.vert), so each variant compiles down to the branches
 * it uses. */
#ifdef GL_FRAGMENT_PRECISION_HIGH
#define TEXCOORD_PRECISION highp
#else
#define TEXCOORD_PRECISION mediump
#endif

#ifndef GEOMETRY
#define GEOMETRY 0
#endif
#ifndef MATERIAL
#define MATERIAL 1
#endif
#ifndef LIGHTMAP
#define LIGHTMAP 0
#endif
/* BitmapCache::type_mask codes, 0 = no map */
#ifndef FORMAT0
#define FORMAT0 0
#endif
#ifndef FORMAT1
#define FORMAT1 0
#endif
#ifndef FORMAT2
#define FORMAT2 0
#endif
#ifndef FORMAT3
#define FORMAT3 0
#endif
#ifndef FORMAT4
#define FORMAT4 0
#endif
#ifndef LIGHTMAP_FORMAT
#define LIGHTMAP_FORMAT 4
#endif
#ifndef REFLECTION
#define REFLECTION 0
#endif
/* Two-channel images upload as luminance-alpha */
#ifndef LA_TEXTURES
#define LA_TEXTURES 0
#endif
/* Material switches; an undefined name in #if is an error in GLSL ES */
#ifndef SENV_TYPE
#define SENV_TYPE 0
#endif
#ifndef MICRO
#define MICRO 0
#endif
#ifndef SELF_ILLUM
#define SELF_ILLUM 0
#endif
#ifndef ALPHA_TESTED
#define ALPHA_TESTED 0
#endif
#ifndef DETAIL_FUNC
#define DETAIL_FUNC 0
#endif
#ifndef MICRO_FUNC
#define MICRO_FUNC 0
#endif
#ifndef DETAIL_MASK
#define DETAIL_MASK 0
#endif
#ifndef BLEND
#define BLEND 0
#endif
#ifndef STAGES
#define STAGES 0
#endif

varying TEXCOORD_PRECISION vec4 frag_uv01;
varying TEXCOORD_PRECISION vec4 frag_uv23;
varying TEXCOORD_PRECISION vec4 frag_uv4l;
varying vec4 frag_light;
varying vec4 frag_reflect;

uniform sampler2D map0;
uniform sampler2D map1;
uniform sampler2D map2;
uniform sampler2D map3;
uniform sampler2D map4;
uniform sampler2D lightmap;
uniform samplerCube cube;
uniform vec4 map_fade[5]; /* average rgb, texels across */

vec4 decode(vec4 t, int format)
{
#if LA_TEXTURES == 1
    float second = t.a;
#else
    float second = t.g;
#endif
    if(format == 4)
        return vec4(t.rgb, 1.0);
    if(format == 5)
        return t.rrrr;
    if(format == 6)
        return vec4(t.rrr, second);
    /* A4R4G4B4 reads as (A, R, G, B) */
    if(format == 7)
        return t.gbar;
    /* A8R8G8B8 bytes are B, G, R, A */
    if(format == 8)
        return t.bgra;
    if(format == 9)
        return vec4(1.0, 1.0, 1.0, t.r);
    if(format == 10)
        return vec4(t.rrr, 1.0);
    if(format == 11)
        return t.rrrr;
    if(format == 12)
        return vec4(t.rrr, second);
    return t;
}

vec4 fetch0()
{
#if FORMAT0 != 0
    return decode(texture2D(map0, frag_uv01.xy), FORMAT0);
#else
    return vec4(1.0);
#endif
}

vec4 fetch1()
{
#if FORMAT1 != 0
    return decode(texture2D(map1, frag_uv01.zw), FORMAT1);
#else
    return vec4(1.0);
#endif
}

vec4 fetch2()
{
#if FORMAT2 != 0
    return decode(texture2D(map2, frag_uv23.xy), FORMAT2);
#else
    return vec4(1.0);
#endif
}

vec4 fetch3()
{
#if FORMAT3 != 0
    return decode(texture2D(map3, frag_uv23.zw), FORMAT3);
#else
    return vec4(1.0);
#endif
}

vec4 fetch4()
{
#if FORMAT4 != 0
    return decode(texture2D(map4, frag_uv4l.xy), FORMAT4);
#else
    return vec4(1.0);
#endif
}

vec3 light_color()
{
    return decode(texture2D(lightmap, frag_uv4l.zw), LIGHTMAP_FORMAT).rgb;
}

/* 0 biased multiply, 1 multiply, 2 biased add */
vec3 apply_detail(vec3 color, vec3 detail, int func)
{
    if(func == 1)
        return color * detail;
    if(func == 2)
        return clamp(color + detail * 2.0 - 1.0, 0.0, 1.0);
    return clamp(color * detail * 2.0, 0.0, 1.0);
}

/* ES2 samples the base level only (no GL_TEXTURE_MAX_LEVEL for a short
 * chain), so a detail map tiled into the distance shimmers. Fade it toward
 * its average, what its mipmaps would have converged on, by how many texels
 * a pixel spans. */
vec3 detail_fade(vec3 detail, TEXCOORD_PRECISION vec2 uv, int map)
{
#ifdef GL_OES_standard_derivatives
    vec2  texels    = fwidth(uv) * map_fade[map].a;
    float footprint = max(max(texels.x, texels.y), 1.0);
    float fade      = clamp(1.0 - log2(footprint) * 0.5, 0.0, 1.0);
    return mix(map_fade[map].rgb, detail, fade);
#else
    return detail;
#endif
}

/* The perpendicular colour at grazing angles, parallel head-on */
vec4 fresnel_tint(vec4 perpendicular, vec4 parallel)
{
    return mix(parallel, perpendicular, frag_reflect.w);
}

#if MATERIAL == 1
/* shader_environment. SENV_TYPE 0 normal, 1 blended, 2 blended specular */
uniform vec4  perpendicular;
uniform vec4  parallel;
uniform float lightmap_brightness;
uniform vec4  self_illum[3]; /* primary, secondary, plasma (a = phase) */

void main()
{
    vec4 base      = fetch0();
    vec4 primary   = fetch2();
    vec4 secondary = fetch3();
#if MICRO
    vec4 micro = fetch1();
#else
    vec4 micro = vec4(1.0);
#endif
#if FORMAT4 != 0
    vec4 bump = fetch4();
#if ALPHA_TESTED
    if(bump.a < 0.05)
        discard;
#endif
#endif

#if SENV_TYPE == 0
    float factor = secondary.a;
#else
    float factor = base.a;
#endif
#if SENV_TYPE == 1 && MICRO
    float specular = micro.a;
#else
    float specular = base.a;
#endif

    vec3 color = base.rgb;
#if FORMAT2 != 0
    primary.rgb = detail_fade(primary.rgb, frag_uv23.xy, 2);
#endif
#if FORMAT3 != 0
    secondary.rgb = detail_fade(secondary.rgb, frag_uv23.zw, 3);
#endif
#if FORMAT2 != 0 && FORMAT3 != 0
    color = apply_detail(color, mix(secondary.rgb, primary.rgb, factor), DETAIL_FUNC);
#elif FORMAT2 != 0
    color = apply_detail(color, primary.rgb, DETAIL_FUNC);
#elif FORMAT3 != 0
    color = apply_detail(color, secondary.rgb, DETAIL_FUNC);
#endif
#if MICRO
    color = apply_detail(
        color, detail_fade(micro.rgb, frag_uv01.zw, 1), MICRO_FUNC);
#endif

#if GEOMETRY == 0
    vec3 light = light_color();
#elif LIGHTMAP == 1
    vec3 light = light_color() * frag_light.rgb;
#else
    vec3 light = frag_light.rgb;
#endif
    color *= light;

#if FORMAT4 != 0 && GEOMETRY == 0
    /* Bump shading is diffuse lighting; it does not scale the reflection */
    vec3  normal      = normalize(bump.rgb * 2.0 - 1.0);
    vec3  key         = normalize(frag_light.xyz);
    float bump_factor = clamp(
        1.0 + (dot(normal, key) - key.z) / max(key.z + 1.0, 0.5), 0.1, 2.0);
    color *= bump_factor;
#endif

#if REFLECTION
    vec4  tint     = fresnel_tint(perpendicular, parallel);
    float lm_luma  = dot(light, vec3(0.299, 0.587, 0.114));
    float strength = tint.a * mix(0.5 + 0.5 * lm_luma, 1.0, lightmap_brightness);
    vec3  cube_rgb = textureCube(cube, frag_reflect.xyz).rgb;
    color = clamp(color + cube_rgb * tint.rgb * strength * specular, 0.0, 1.0);
#endif

#if SELF_ILLUM
    vec4  illum = fetch1();
    float plasma =
        illum.b * max(0.0, 1.0 - abs(self_illum[2].a - illum.a));
    color += illum.r * self_illum[0].rgb + illum.g * self_illum[1].rgb +
             plasma * self_illum[2].rgb;
#endif

    gl_FragColor = vec4(color, 1.0);
}
#endif

#if MATERIAL == 4 || MATERIAL == 5
/* shader_chicago(_extended): up to three combiner stages folding the maps
 * left to right. C1..C3 / A1..A3 are the colour/alpha functions. */
#ifndef C1
#define C1 0
#define A1 0
#define C2 0
#define A2 0
#define C3 0
#define A3 0
#endif
vec4 chicago_stage(vec4 i1, vec4 last, vec4 i2, int cf, int af)
{
    vec4 dst = i1;
    if(cf == 1)
        dst.rgb = i2.rgb;
    else if(cf == 2)
        dst.rgb = i1.rgb * i2.rgb;
    else if(cf == 3)
        dst.rgb = i1.rgb * i2.rgb * 2.0;
    else if(cf >= 4 && cf <= 6)
        dst.rgb = i1.rgb + i2.rgb;
    else if(cf == 7 || cf == 8)
        dst.rgb = i1.rgb - i2.rgb;
    else if(cf == 9)
        dst.rgb = i1.rgb + i2.rgb * last.a;
    else if(cf == 10)
        dst.rgb = i1.rgb + i2.rgb * (1.0 - last.a);
    else if(cf == 11)
        dst.rgb = i1.rgb + i2.rgb * i2.a;
    else if(cf == 12)
        dst.rgb = i1.rgb + i2.rgb * (1.0 - i2.a);

    if(af == 2)
        dst.a = i1.a * i2.a;
    else if(af == 3)
        dst.a = i1.a * i2.a * 2.0;
    else if(af >= 4 && af <= 6)
        dst.a = i1.a + i2.a;
    else if(af == 7 || af == 8)
        dst.a = i1.a - i2.a;
    else if(af == 9)
        dst.a = i1.a * last.a;
    else if(af == 10)
        dst.a = i1.a * (1.0 - last.a);
    else if(af == 11)
        dst.a = i1.a + i2.a * last.a;
    else if(af == 12)
        dst.a = i1.a + i2.a * (1.0 - i2.a);
    return dst;
}

void main()
{
    vec4 c1 = fetch0();
    vec4 c2 = fetch1();
    vec4 c3 = fetch2();
    vec4 c4 = fetch3();

    vec4 color = c1;
#if FORMAT1 != 0
    color = chicago_stage(c1, c1, c2, C1, A1);
#endif
#if FORMAT2 != 0
    color = chicago_stage(color, c2, c3, C2, A2);
#endif
#if FORMAT3 != 0
    color = chicago_stage(color, c3, c4, C3, A3);
#endif
    /* add, component min/max ignore alpha */
#if BLEND == 3 || BLEND == 5 || BLEND == 6
    color.a = 1.0;
#endif
    gl_FragColor = color;
}
#endif

#if MATERIAL == 7
/* shader_transparent: the register combiners, every stage's routing a
 * constant (S0..S6, see LegacyMaterials::transparent_constants). Registers:
 * r1/r2 scratch (r1 final), r3/r4 vertex colours (1 = no fade), r5..r8 maps */
uniform vec4 color0[7]; /* animated constant_color0 per stage */

vec4 r1, r2, r3, r4, r5, r6, r7, r8;

vec4 reg(int i)
{
    if(i == 1) return r1;
    if(i == 2) return r2;
    if(i == 3) return r3;
    if(i == 4) return r4;
    if(i == 5) return r5;
    if(i == 6) return r6;
    if(i == 7) return r7;
    if(i == 8) return r8;
    return vec4(0.0);
}

void set_rgb(int i, vec3 v)
{
    if(i == 1) r1.rgb = v;
    else if(i == 2) r2.rgb = v;
    else if(i == 3) r3.rgb = v;
    else if(i == 4) r4.rgb = v;
    else if(i == 5) r5.rgb = v;
    else if(i == 6) r6.rgb = v;
    else if(i == 7) r7.rgb = v;
    else if(i == 8) r8.rgb = v;
}

void set_a(int i, float v)
{
    if(i == 1) r1.a = v;
    else if(i == 2) r2.a = v;
    else if(i == 3) r3.a = v;
    else if(i == 4) r4.a = v;
    else if(i == 5) r5.a = v;
    else if(i == 6) r6.a = v;
    else if(i == 7) r7.a = v;
    else if(i == 8) r8.a = v;
}

vec3 cin(int i, vec4 c0, vec4 c1)
{
    if(i == 1) return vec3(1.0);
    if(i == 2) return vec3(0.5);
    if(i == 3) return vec3(-1.0);
    if(i == 4) return vec3(-0.5);
    if(i >= 5 && i <= 8) return reg(i).rgb;
    if(i == 9 || i == 10) return reg(i - 6).rgb;
    if(i == 11 || i == 12) return reg(i - 10).rgb;
    if(i == 13) return c0.rgb;
    if(i == 14) return c1.rgb;
    if(i >= 15 && i <= 18) return vec3(reg(i - 10).a);
    if(i == 19 || i == 20) return vec3(reg(i - 16).a);
    if(i == 21 || i == 22) return vec3(reg(i - 20).a);
    if(i == 23) return vec3(c0.a);
    if(i == 24) return vec3(c1.a);
    return vec3(0.0);
}

/* Alpha inputs: 15..24 read blue where colour inputs read alpha */
float ain(int i, vec4 c0, vec4 c1)
{
    if(i == 1) return 1.0;
    if(i == 2) return 0.5;
    if(i == 3) return -1.0;
    if(i == 4) return -0.5;
    if(i >= 5 && i <= 8) return reg(i).a;
    if(i == 9 || i == 10) return reg(i - 6).a;
    if(i == 11 || i == 12) return reg(i - 10).a;
    if(i == 13) return c0.a;
    if(i == 14) return c1.a;
    if(i >= 15 && i <= 18) return reg(i - 10).b;
    if(i == 19 || i == 20) return reg(i - 16).b;
    if(i == 21 || i == 22) return reg(i - 20).b;
    if(i == 23) return c0.b;
    if(i == 24) return c1.b;
    return 0.0;
}

vec3 cmap(vec3 v, int m)
{
    vec3 u = max(v, 0.0);
    if(m == 1) return 1.0 - clamp(v, 0.0, 1.0);
    if(m == 2) return 2.0 * u - 1.0;
    if(m == 3) return 1.0 - 2.0 * u;
    if(m == 4) return u - 0.5;
    if(m == 5) return 0.5 - u;
    if(m == 6) return v;
    if(m == 7) return -v;
    return u;
}

float amap(float v, int m)
{
    float u = max(v, 0.0);
    if(m == 1) return 1.0 - clamp(v, 0.0, 1.0);
    if(m == 2) return 2.0 * u - 1.0;
    if(m == 3) return 1.0 - 2.0 * u;
    if(m == 4) return u - 0.5;
    if(m == 5) return 0.5 - u;
    if(m == 6) return v;
    if(m == 7) return -v;
    return u;
}

vec3 omap(vec3 v, int m)
{
    if(m == 1) return v * 0.5;
    if(m == 2) return v * 2.0;
    if(m == 3) return v * 4.0;
    if(m == 4) return v - 0.5;
    if(m == 5) return v * 2.0 - 1.0;
    return v;
}

float oamap(float v, int m)
{
    if(m == 1) return v * 0.5;
    if(m == 2) return v * 2.0;
    if(m == 3) return v * 4.0;
    if(m == 4) return v - 0.5;
    if(m == 5) return v * 2.0 - 1.0;
    return v;
}

/* Inputs a..d as (register, mapping) for colour then alpha, the colour
 * outputs (ab, ab dot, cd, cd dot, sum, map), the alpha outputs (ab, cd, sum,
 * map), the stage flags */
void stage(
    int ca, int cam, int cb, int cbm, int cc, int ccm, int cd, int cdm,
    int aa, int aam, int ab, int abm, int ac, int acm, int ad, int adm,
    int c_ab_d, int c_ab_fn, int c_cd_d, int c_cd_fn, int c_sum_d, int c_om,
    int a_ab_d, int a_cd_d, int a_sum_d, int a_om, int flags,
    vec4 c0, vec4 c0_up, vec4 c1)
{
    /* With a_out_controls_color0_anim the scratch alpha drives the ramp */
    if(flags >= 4)
        c0 = mix(c0, c0_up, clamp(r1.a, 0.0, 1.0));
    bool color_mux = flags == 1 || flags == 3 || flags == 5 || flags == 7;
    bool alpha_mux = flags == 2 || flags == 3 || flags == 6 || flags == 7;

    float a_ab  = amap(ain(aa, c0, c1), aam) * amap(ain(ab, c0, c1), abm);
    float a_cd  = amap(ain(ac, c0, c1), acm) * amap(ain(ad, c0, c1), adm);
    float a_sum = alpha_mux ? (r1.a >= 0.5 ? a_cd : a_ab) : a_ab + a_cd;

    vec3 va   = cmap(cin(ca, c0, c1), cam);
    vec3 vb   = cmap(cin(cb, c0, c1), cbm);
    vec3 vc   = cmap(cin(cc, c0, c1), ccm);
    vec3 vd   = cmap(cin(cd, c0, c1), cdm);
    vec3 c_ab = c_ab_fn == 0 ? va * vb : vec3(dot(va, vb));
    vec3 c_cd = c_cd_fn == 0 ? vc * vd : vec3(dot(vc, vd));
    vec3 c_sum = color_mux ? (r1.a >= 0.5 ? c_cd : c_ab) : c_ab + c_cd;

    /* Registers clamp to [-1, 1] on write */
    set_rgb(c_ab_d, clamp(omap(c_ab, c_om), -1.0, 1.0));
    set_rgb(c_cd_d, clamp(omap(c_cd, c_om), -1.0, 1.0));
    set_rgb(c_sum_d, clamp(omap(c_sum, c_om), -1.0, 1.0));
    set_a(a_ab_d, clamp(oamap(a_ab, a_om), -1.0, 1.0));
    set_a(a_cd_d, clamp(oamap(a_cd, a_om), -1.0, 1.0));
    set_a(a_sum_d, clamp(oamap(a_sum, a_om), -1.0, 1.0));
}

void main()
{
    r1 = vec4(0.0);
    r2 = vec4(0.0);
    r3 = vec4(1.0);
    r4 = vec4(1.0);
    r5 = fetch0();
    r6 = fetch1();
    r7 = fetch2();
    r8 = fetch3();

#if STAGES == 0
    r1 = r5;
#endif
#if STAGES > 0
    stage(S0, color0[0], S0_C0_UP, S0_C1);
#endif
#if STAGES > 1
    stage(S1, color0[1], S1_C0_UP, S1_C1);
#endif
#if STAGES > 2
    stage(S2, color0[2], S2_C0_UP, S2_C1);
#endif
#if STAGES > 3
    stage(S3, color0[3], S3_C0_UP, S3_C1);
#endif
#if STAGES > 4
    stage(S4, color0[4], S4_C0_UP, S4_C1);
#endif
#if STAGES > 5
    stage(S5, color0[5], S5_C0_UP, S5_C1);
#endif
#if STAGES > 6
    stage(S6, color0[6], S6_C0_UP, S6_C1);
#endif

    vec4 color = r1;
#if BLEND == 3 || BLEND == 5 || BLEND == 6
    color.a = 1.0;
#endif
    gl_FragColor = color;
}
#endif

#if MATERIAL == 9
/* shader_model. Multipurpose map: specular b, self-illumination g, colour
 * change a (Xbox: r, g, b). DETAIL_MASK picks what masks the detail map. */
uniform vec4 change_color;
uniform vec4 perpendicular;
uniform vec4 parallel;

void main()
{
    vec4 color = fetch0();
    if(color.a < 0.5)
        discard;

#if FORMAT1 != 0
    vec4 multi = fetch1();
#else
    vec4 multi = vec4(0.0);
#endif
#ifdef VERTEX_XBOX
    float specular_factor = multi.r;
    float illum_factor    = multi.g;
    float color_change    = multi.b;
#else
    float specular_factor = multi.b;
    float illum_factor    = multi.g;
    float color_change    = multi.a;
#endif

#if LIGHTMAP == 1
    vec3 lit = clamp(light_color() * frag_light.rgb, 0.1, 1.0);
#else
    vec3 lit = frag_light.rgb;
#endif

    color.rgb *= mix(vec3(1.0), change_color.rgb, color_change * change_color.a);
    color.rgb = clamp(lit * color.rgb + color.rgb * illum_factor, 0.0, 1.0);

#if REFLECTION
    vec4 tint   = fresnel_tint(perpendicular, parallel);
    vec3 refl   = textureCube(cube, frag_reflect.xyz).rgb * tint.rgb;
    float shine = clamp(tint.a * specular_factor, 0.0, 1.0);
    color.rgb   = mix(color.rgb, refl, shine * 0.8);
#endif

#if FORMAT2 != 0
    vec3 detail = detail_fade(fetch2().rgb, frag_uv23.xy, 2);
#if DETAIL_MASK == 1
    float detail_factor = specular_factor;
#elif DETAIL_MASK == 2
    float detail_factor = 1.0 - specular_factor;
#elif DETAIL_MASK == 3
    float detail_factor = color_change;
#elif DETAIL_MASK == 4
    float detail_factor = 1.0 - color_change;
#elif DETAIL_MASK == 5
    float detail_factor = illum_factor;
#elif DETAIL_MASK == 6
    float detail_factor = 1.0 - illum_factor;
#elif DETAIL_MASK == 7
    float detail_factor = multi.a;
#elif DETAIL_MASK == 8
    float detail_factor = 1.0 - multi.a;
#else
    float detail_factor = 1.0;
#endif
    color.rgb = clamp(
        mix(color.rgb, apply_detail(color.rgb, detail, DETAIL_FUNC), detail_factor),
        0.0, 1.0);
#endif
    gl_FragColor = color;
}
#endif
