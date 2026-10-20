/* Halo 2 BSP/model fragment body. One program per material family, chosen
 * by the defines a variant file sets before including this:
 *   H2_SKY    dome layer: unlit, alpha from the base map (H2_SKY_MASK: times
 *             a cloud mask), follows the camera
 *   H2_WATER  water_pool: bump map in `diffuse`, fresnel-mixed tint
 *   H2_DETAIL 0 none, 1 detail map modulated x2, 2 two blend details mixed
 *             by the base alpha
 *   H2_ILLUM  self-illumination map added, tinted by its pass colour
 *   H2_CLOUDS transparent clouds template: unlit, alpha from map and mask
 *   H2_OCEAN  water_static: scrolling bump in `diffuse`, sky reflection in
 *             `detail`, fresnel-mixed, faded by `mask`
 *   H2_PLASMA plasma_alpha: bright where noise A meets the drifting noise B,
 *             shaped by `mask`, ramped between two colours */

#ifndef H2_CLOUDS
#define H2_CLOUDS 0
#endif
#ifndef H2_OCEAN
#define H2_OCEAN 0
#endif
#ifndef H2_PLASMA
#define H2_PLASMA 0
#endif

#include "render_flags.glsl"

/* Debug views from RenderingParameters */
layout(location=55) uniform int render_flags;

layout(location=0) in vec3 f_world_pos;
layout(location=1) in vec2 f_texcoord;
layout(location=2) in vec3 f_normal;
layout(location=3) in vec2 f_detail_uv;
layout(location=4) in vec2 f_detail2_uv;
layout(location=5) in vec2 f_illum_uv;
layout(location=6) in vec3 f_lightmap;
layout(location=8) in vec4 f_vertex_light;
layout(location=7) in vec2 f_mask_uv;
layout(location=9) in vec2 f_bump_uv;

layout(location=2, binding=0) uniform sampler2D diffuse;
layout(location=3) uniform int alpha_test;
#if H2_DETAIL >= 1
layout(location=9, binding=1) uniform sampler2D detail;
#endif
#if H2_DETAIL == 2
layout(location=10, binding=2) uniform sampler2D detail2;
#endif
#if H2_ILLUM
layout(location=13, binding=3) uniform sampler2D illum;
layout(location=15) uniform vec3 illum_color;
#endif
#if !H2_SKY && !H2_WATER
layout(location=14, binding=4) uniform sampler2DArray lightmaps;
layout(location=29) uniform int bumped;
layout(location=30, binding=6) uniform sampler2D bump;
/* Environment reflection: facing and glancing tint, brightness in alpha */
layout(location=36, binding=7) uniform samplerCube env;
layout(location=37) uniform vec4 env_facing;
layout(location=38) uniform vec4 env_glancing;
#endif
#if H2_SKY_MASK || H2_CLOUDS || H2_OCEAN || H2_PLASMA
layout(location=18, binding=5) uniform sampler2D mask;
#endif
#if !H2_SKY
layout(location=21) uniform vec3 camera_position;
/* The sky's light: glints off the waves, shadows of moving objects */
layout(location=43) uniform vec3 sun_direction;
#endif
#if !H2_SKY && !H2_WATER
#include "cascaded_shadows.glsl"
#endif
#if H2_OCEAN || H2_PLASMA
layout(location=31) uniform float time;
#endif
#if H2_OCEAN
/* The opaque scene's depth, to find the ground under the water */
layout(location=40, binding=8) uniform sampler2D scene_depth;
/* 1: stored depth is clip space z; 0: (z + 1) / 2, as without clip control */
layout(location=41) uniform int depth_zero_to_one;
layout(location=50) uniform mat4 inverse_camera; /* 50-53 */
layout(location=54) uniform vec2 viewport_size;
layout(location=44) uniform vec3 sun_color;
/* The water definition's sun spot: rgb, specular power */
layout(location=48) uniform vec4 sun_spot;
#endif
#if H2_PLASMA
layout(location=32) uniform vec3 plasma_colors[2]; /* bright, dim */
#endif
#if !H2_SKY
/* Atmospheric fog of the camera's cluster: rgb + max density, start and
 * opaque distance; linear in between */
layout(location=22) uniform vec4 fog_color;
layout(location=23) uniform vec2 fog_range;
layout(location=24) uniform vec3 fog_eye;

/* Planar fog of the active section: the volume under the plane, opaque
 * `opaque.x` along the ray through it and `opaque.y` below the surface */
layout(location=25) uniform vec4 planar_fog_plane;
layout(location=26) uniform vec4 planar_fog_color;
layout(location=27) uniform vec2 planar_fog_opaque;

vec3 fogged(vec3 color)
{
    if((render_flags & RENDER_FLAG_FOG) == 0)
        return color;
    float d = distance(fog_eye, f_world_pos);
    float f = clamp((d - fog_range.x) / max(fog_range.y - fog_range.x, 1e-3), 0.0, 1.0);
    color = mix(color, fog_color.rgb, f * fog_color.a);

    /* Depth below the plane at the eye and the pixel; the density ramps
     * from the surface to full at opaque.y below it, integrated along the
     * part of the ray that is under the plane */
    float de = planar_fog_plane.w - dot(planar_fog_plane.xyz, fog_eye);
    float dp = planar_fog_plane.w - dot(planar_fog_plane.xyz, f_world_pos);
    float t0 = de > 0.0 ? 0.0 : (dp > 0.0 ? de / (de - dp) : 1.0);
    float t1 = dp > 0.0 ? 1.0 : (de > 0.0 ? de / (de - dp) : 0.0);
    float len = max(t1 - t0, 0.0) * d;
    float depth0 = max(mix(de, dp, t0), 0.0);
    float depth1 = max(mix(de, dp, t1), 0.0);
    float opaque_depth = max(planar_fog_opaque.y, 1e-3);
    float density = 0.5 * (clamp(depth0 / opaque_depth, 0.0, 1.0) +
                           clamp(depth1 / opaque_depth, 0.0, 1.0));
    /* No opaque distance: opacity by depth below the plane alone */
    float p = planar_fog_opaque.x > 0.0
        ? 1.0 - exp(-density * len / planar_fog_opaque.x)
        : clamp(max(depth0, depth1) / opaque_depth, 0.0, 1.0);
    p = min(p, planar_fog_color.a);
    return mix(color, planar_fog_color.rgb, p);
}
#endif

layout(location=0) out vec4 out_color;

vec3 surface_normal()
{
    /* Sections without a usable normal stream fall back to the face normal */
    return length(f_normal) > 0.5
        ? normalize(f_normal)
        : normalize(cross(dFdx(f_world_pos), dFdy(f_world_pos)));
}

#if !H2_SKY && !H2_WATER
/* Normal map in the frame of the uv gradients, which is how the stored
 * tangent (dP/du) and binormal (dP/dv) are laid out */
vec3 bumped_normal(vec3 n)
{
    vec2 b  = texture(bump, f_bump_uv).rg * 2.0 - 1.0;
    vec3 tn = vec3(b, sqrt(max(1.0 - dot(b, b), 0.0)));
    vec3 dp1 = dFdx(f_world_pos), dp2 = dFdy(f_world_pos);
    vec2 duv1 = dFdx(f_bump_uv), duv2 = dFdy(f_bump_uv);
    vec3 dp2perp = cross(dp2, n), dp1perp = cross(n, dp1);
    vec3 t = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 bt = dp2perp * duv1.y + dp1perp * duv2.y;
    float s = inversesqrt(max(max(dot(t, t), dot(bt, bt)), 1e-20));
    return normalize(mat3(t * s, bt * s, n) * tn);
}

float sun_shade(vec3 n)
{
    vec3 sun = normalize(vec3(0.3, 0.2, 1.0));
    return 0.45 + 0.55 * abs(dot(n, sun));
}
#endif

void main()
{
#if H2_PLASMA
    float a = texture(diffuse, f_texcoord).r;
    float b = texture(detail, f_detail_uv + time * vec2(0.03, -0.21)).r;
    float p = clamp(1.0 - abs(a - b) * 3.0, 0.0, 1.0);
    p *= p;
    vec3 col = mix(plasma_colors[1], plasma_colors[0], p * p);
    out_color = vec4(col * (0.4 + 1.6 * p), texture(mask, f_mask_uv).a * p);
#elif H2_OCEAN
    /* Two wave layers drifting apart, sky seen along the reflection */
    vec2 w0 = texture(diffuse, f_texcoord + time * vec2(0.013, 0.007)).rg;
    vec2 w1 = texture(diffuse, f_texcoord * 1.7 - time * vec2(0.009, 0.011)).rg;
    vec2 w2 = texture(diffuse, f_texcoord * 4.3 + time * vec2(-0.021, 0.017)).rg;
    vec2 slope = (w0 + w1 - 1.0) + (w2 - 0.5) * 0.6;
    vec3 n = normalize(vec3(slope * 1.4, 1.0));
    vec3 v = normalize(camera_position - f_world_pos);
    vec3 r = reflect(-v, n);
    float fresnel = 0.04 + 0.96 * pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 4.0);
    vec2 sky_uv = r.xy / (1.0 + abs(r.z)) * 0.5 + 0.5;
    vec3 sky = texture(detail, sky_uv).rgb;
    /* The reflection photo is bluer than the map's sky: keep its detail */
    vec3 refl = mix(vec3(dot(sky, vec3(0.3, 0.59, 0.11))), sky, 0.25) * 0.85;
    vec3 deep = vec3(0.10, 0.12, 0.12);
    float glance = clamp(fresnel * 1.6, 0.0, 1.0);
    vec3 col = mix(deep, refl, clamp(glance + 0.15, 0.0, 1.0));
    /* The edge blend plane fades out toward the open sea's fan */
    float fade = texture(mask, f_mask_uv).r;
    /* How deep the ground is under this pixel; the shallows thin out to the
     * sand, and nothing drawn (reversed z clears to 0) is open sea */
    float ground = texelFetch(scene_depth, ivec2(gl_FragCoord.xy), 0).r;
    float ndc_z  = depth_zero_to_one != 0 ? ground : ground * 2.0 - 1.0;
    vec4  under  = inverse_camera *
                  vec4(gl_FragCoord.xy / viewport_size * 2.0 - 1.0, ndc_z, 1.0);
    float depth  = ground > 0.0 ? f_world_pos.z - under.z / under.w : 100.0;
    /* Light crosses depth / cos(view) of water: shallows looked down into
     * stay clear, the same shallows far off turn opaque */
    float path    = max(depth, 0.0) / max(v.z, 0.03);
    float shallow = 1.0 - exp(-path * 0.8);
    /* Sun glints: a tight sparkle on the facets and a broad sheen */
    float facing = max(dot(r, sun_direction), 0.0);
    float sparkle = pow(facing, 300.0) * 2.0;
    vec3  spot    = sun_spot.rgb * pow(facing, max(sun_spot.a, 1.0));
    float glint   = sparkle + dot(spot, vec3(0.33));
    col += sun_color * sparkle + spot;
    float alpha = mix(0.6, 1.0, glance) * shallow;
    out_color = vec4(fogged(col), clamp(alpha + glint * shallow, 0.0, 1.0) * fade);
#elif H2_WATER
    vec3 normal = surface_normal();
    /* Bump-perturbed surface normal, sky tint by fresnel, ground through */
    vec3 bump = normalize(vec3(texture(diffuse, f_texcoord).rg * 2.0 - 1.0, 1.5));
    vec3 n = normalize(normal * bump.z + vec3(bump.xy, 0.0));
    vec3 v = normalize(camera_position - f_world_pos);
    float fresnel = pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 3.0);
    vec3 deep = vec3(0.18, 0.26, 0.3);
    vec3 sky = vec3(0.7, 0.78, 0.88);
    vec3 col = mix(deep, sky, fresnel) * (0.85 + 0.3 * bump.z);
    out_color = vec4(fogged(col), mix(0.45, 0.9, fresnel));
#else
    vec4 base = texture(diffuse, f_texcoord);
    if(alpha_test != 0 && base.a < 0.5)
        discard;
    vec3 color = base.rgb;
    if((render_flags & RENDER_FLAG_ONLY_DIFFUSE) != 0)
    {
        out_color = vec4(color, 1.0);
        return;
    }
#if H2_DETAIL >= 1
    if((render_flags & RENDER_FLAG_ONLY_DETAIL) != 0)
    {
        out_color = vec4(texture(detail, f_detail_uv).rgb, 1.0);
        return;
    }
#endif
#if H2_CLOUDS
    vec4 m = texture(mask, f_mask_uv);
    out_color = vec4(fogged(color * m.rgb), base.a * m.a);
    return;
#endif
#if H2_DETAIL == 1
    color *= texture(detail, f_detail_uv).rgb * 2.0;
#elif H2_DETAIL == 2
    color *= mix(
        texture(detail, f_detail_uv).rgb,
        texture(detail2, f_detail2_uv).rgb,
        base.a) * 2.0;
#endif
#if H2_SKY
    float alpha = base.a;
#if H2_SKY_MASK
    vec4 m = texture(mask, f_mask_uv);
    alpha *= m.a * m.r;
#endif
#else
    /* Baked lighting where the geometry has it, the sun stand-in elsewhere;
     * baked light has no direction, so the bump shades relative to the sun */
    vec3 normal = surface_normal();
    vec3 shading_normal = bumped != 0 ? bumped_normal(normal) : normal;
    if((render_flags & RENDER_FLAG_ONLY_NORMALS) != 0)
    {
        out_color = vec4(normal * 0.5 + 0.5, 1.0);
        return;
    }
    if((render_flags & RENDER_FLAG_ONLY_NORMALMAP) != 0)
    {
        out_color = vec4(shading_normal * 0.5 + 0.5, 1.0);
        return;
    }
    float bump_shade = sun_shade(shading_normal) / sun_shade(normal);
    vec3 light = (render_flags & RENDER_FLAG_LIGHTMAP) == 0 ? vec3(1.0)
               : f_lightmap.z >= 0.0 ? texture(lightmaps, f_lightmap).rgb * 2.0
               : f_vertex_light.a > 0.0 ? f_vertex_light.rgb * 2.0
               : vec3(sun_shade(normal));
    /* A cast shadow takes the sun's share out of the baked light: none
     * where it is dim (shade the lightmap already holds) */
    float sunlit = smoothstep(0.15, 0.45, dot(light, vec3(0.299, 0.587, 0.114)));
    light *= 1.0 - cascade_occlusion(f_world_pos, normal, sun_direction, camera_position) *
                       csm_params.y * sunlit;
    if((render_flags & RENDER_FLAG_ONLY_LIGHTMAP) != 0)
    {
        out_color = vec4(light, 1.0);
        return;
    }
    color *= light * bump_shade;
    bool only_env = (render_flags & RENDER_FLAG_ONLY_REFLECTIONS) != 0;
    if(only_env)
        color = vec3(0.0);
    /* Reflection masked by the base map's alpha, lit like the surface */
    if((render_flags & RENDER_FLAG_REFLECTION) != 0 &&
       env_facing.a + env_glancing.a > 0.0)
    {
        vec3 v = normalize(camera_position - f_world_pos);
        vec3 r = reflect(-v, shading_normal);
        float glance = pow(1.0 - clamp(dot(shading_normal, v), 0.0, 1.0), 2.0);
        vec4 tint = mix(env_facing, env_glancing, glance);
        color += texture(env, vec3(-r.y, r.z, r.x)).rgb * tint.rgb * tint.a *
                 base.a * light;
    }
#if H2_ILLUM
    if(!only_env)
        color += texture(illum, f_illum_uv).rgb * illum_color;
#endif
    color = only_env ? color : fogged(color);
    float alpha = 1.0;
#endif
    out_color = vec4(color, alpha);
#endif
}
