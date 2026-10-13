/* Halo 2 BSP/model fragment body. One program per material family, chosen
 * by the defines a variant file sets before including this:
 *   H2_SKY    dome layer: unlit, alpha from the base map (H2_SKY_MASK: times
 *             a cloud mask), follows the camera
 *   H2_WATER  water_pool: bump map in `diffuse`, fresnel-mixed tint
 *   H2_DETAIL 0 none, 1 detail map modulated x2, 2 two blend details mixed
 *             by the base alpha
 *   H2_ILLUM  self-illumination map added, tinted by its pass colour */

layout(location=0) in vec3 f_world_pos;
layout(location=1) in vec2 f_texcoord;
layout(location=2) in vec3 f_normal;
layout(location=3) in vec2 f_detail_uv;
layout(location=4) in vec2 f_detail2_uv;
layout(location=5) in vec2 f_illum_uv;
layout(location=6) in vec3 f_lightmap;
layout(location=8) in vec4 f_vertex_light;
layout(location=7) in vec2 f_mask_uv;

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
#endif
#if H2_SKY_MASK
layout(location=18, binding=5) uniform sampler2D mask;
#endif
#if H2_WATER
layout(location=21) uniform vec3 camera_position;
#endif

layout(location=0) out vec4 out_color;

vec3 surface_normal()
{
    /* Sections without a usable normal stream fall back to the face normal */
    return length(f_normal) > 0.5
        ? normalize(f_normal)
        : normalize(cross(dFdx(f_world_pos), dFdy(f_world_pos)));
}

void main()
{
#if H2_WATER
    vec3 normal = surface_normal();
    /* Bump-perturbed surface normal, sky tint by fresnel, ground through */
    vec3 bump = normalize(vec3(texture(diffuse, f_texcoord).rg * 2.0 - 1.0, 1.5));
    vec3 n = normalize(normal * bump.z + vec3(bump.xy, 0.0));
    vec3 v = normalize(camera_position - f_world_pos);
    float fresnel = pow(1.0 - clamp(dot(n, v), 0.0, 1.0), 3.0);
    vec3 deep = vec3(0.18, 0.26, 0.3);
    vec3 sky = vec3(0.7, 0.78, 0.88);
    vec3 col = mix(deep, sky, fresnel) * (0.85 + 0.3 * bump.z);
    out_color = vec4(col, mix(0.45, 0.9, fresnel));
#else
    vec4 base = texture(diffuse, f_texcoord);
    if(alpha_test != 0 && base.a < 0.5)
        discard;
    vec3 color = base.rgb;
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
    /* Baked lighting where the geometry has it, the sun stand-in elsewhere */
    if(f_lightmap.z >= 0.0)
        color *= texture(lightmaps, f_lightmap).rgb * 2.0;
    else if(f_vertex_light.a > 0.0)
        color *= f_vertex_light.rgb * 2.0;
    else
    {
        vec3 normal = surface_normal();
        vec3 sun = normalize(vec3(0.3, 0.2, 1.0));
        color *= 0.45 + 0.55 * abs(dot(normal, sun));
    }
#if H2_ILLUM
    color += texture(illum, f_illum_uv).rgb * illum_color;
#endif
    float alpha = 1.0;
#endif
    out_color = vec4(color, alpha);
#endif
}
