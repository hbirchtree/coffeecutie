#version 460 core

#extension GL_GOOGLE_include_directive : enable

/* shader_water (water_shading.fx): the scene behind refracted by the wave
 * slopes and fogged by depth, a static cube reflection by fresnel, a little
 * diffuse, bank alpha into the shallows. Output is linear and exposed. */

layout(location=0) in vec3 f_world_pos;
layout(location=1) in vec2 f_texcoord;
layout(location=2) in vec3 f_normal;
layout(location=6) in vec3 f_lightmap;

#include "fragments/h3_atmosphere.glsl"

/* Per loaded structure BSP (the lightmap z), ten layers each */
layout(location=194) uniform vec4 lightprobe_scales[8];
layout(location=91) uniform float exposure;
layout(location=89, binding=9) uniform sampler2DArray lightprobe;

layout(location=130) uniform float time;
layout(location=131) uniform vec4 slope_xform;
layout(location=132) uniform vec4 shape_xform;
/* slope range xy, detail slope scale xy */
layout(location=133) uniform vec4 slope;
/* wave height, aux height, detail steepness, minimal disturbance */
layout(location=134) uniform vec4 heights;
/* watercolor coefficient, reflection coefficient, fresnel r0, murkiness */
layout(location=135) uniform vec4 shading;
/* refraction shift, extinct distance, depth dominant ratio, bank depth */
layout(location=136) uniform vec4 refraction;
/* sunspot cut, shadow intensity mark, slope layers, has reflection */
layout(location=137) uniform vec4 misc;
layout(location=138) uniform vec4 water_diffuse;
layout(location=139) uniform vec2 viewport;
/* Decode exponents: watercolor, reflection */
layout(location=144) uniform vec2 water_curves;

layout(location=140, binding=0) uniform sampler2D water_color;
layout(location=141, binding=1) uniform samplerCube water_reflection;
layout(location=142, binding=2) uniform sampler2DArray water_slopes;
layout(location=143, binding=3) uniform sampler2D scene_copy;
layout(location=145, binding=4) uniform sampler2D foam_texture;
layout(location=146, binding=5) uniform sampler2D foam_detail;
/* global_shape_texture: painted foam in b */
layout(location=147, binding=6) uniform sampler2D global_shape;
layout(location=148, binding=7) uniform sampler2DArray wave_displacement;
layout(location=149) uniform vec4 foam_xform;
layout(location=150) uniform vec4 foam_detail_xform;
/* foam height, foam pow, 1 auto / 2 paint / 3 both, displacement frames */
layout(location=151) uniform vec4 foam;
layout(location=152) uniform vec4 displacement_range;

layout(location=0) out vec4 out_color;

/* The slope array is a loop of wave frames; between two of them */
vec2 wave_slope(vec2 uv)
{
    float frame = fract(time * 0.5) * misc.z;
    float a     = floor(frame);
    vec2  s0    = texture(water_slopes, vec3(uv, a)).rg;
    vec2  s1    = texture(water_slopes, vec3(uv, mod(a + 1.0, misc.z))).rg;
    /* restore_slope: [0, 1] to [-range/2, range/2] */
    return (mix(s0, s1, frame - a) - 0.5) * slope.xy;
}

/* The displacement loop's height channel, [0, 1] */
float wave_height(vec2 uv)
{
    float frame = fract(time * 0.5) * foam.w;
    float a     = floor(frame);
    float z0    = texture(wave_displacement, vec3(uv, a)).b;
    float z1    = texture(wave_displacement, vec3(uv, mod(a + 1.0, foam.w))).b;
    return mix(z0, z1, frame - a);
}

/* Foam: auto where the waves crest past foam_height, painted from the
 * shape texture, or both */
float foam_factor(vec2 uv_main, vec2 uv_aux)
{
    float factor = 0.0;
    if((int(foam.z) & 1) != 0 && foam.w > 0.0)
    {
        /* restore_displacement, both layers, from texture to geometry space */
        float range      = displacement_range.z;
        float wave_scale = sqrt(max(shape_xform.x * shape_xform.y, 1e-6));
        float height     = ((wave_height(uv_main) - 0.5) * range * heights.x +
                            (wave_height(uv_aux) - 0.5) * range * heights.y) / wave_scale;
        float highest    = 0.5 * range * (heights.x + heights.y) / wave_scale;
        float span       = clamp(highest - foam.x, 0.0, 1.0);
        if(span > 0.0)
            factor = pow(clamp((height - foam.x) / span, 0.0, 1.0), foam.y);
    }
    if((int(foam.z) & 2) != 0)
        factor = max(factor, texture(global_shape, f_texcoord).b);
    return factor;
}

float fog_factor(float murkiness, float depth)
{
    return 1.0 - clamp(1.0 / exp(murkiness * depth), 0.0, 1.0);
}

void main()
{
    vec2 uv_main   = f_texcoord * shape_xform.xy + shape_xform.zw;
    vec2 uv_aux    = f_texcoord * slope_xform.xy + slope_xform.zw;
    vec2 uv_detail = uv_main * slope.zw;
    vec2 s_main    = wave_slope(uv_main);
    vec2 s_aux     = wave_slope(uv_aux) * heights.y + wave_slope(uv_detail) * heights.z;
    vec2 slope_shading    = s_main * heights.x + s_aux;
    vec2 slope_refraction = s_main * max(heights.x, heights.w) + s_aux;

    /* Water lies flat in world space here; the slopes tilt +z */
    vec3 normal = normalize(vec3(slope_shading, 1.0));

    vec3 lightmap_intensity = vec3(1.0);
    if(f_lightmap.z >= 0.0)
    {
        int  slot = int(f_lightmap.z + 0.5);
        vec4 a = texture(lightprobe, vec3(f_lightmap.xy, float(slot * 10)));
        vec4 b = texture(lightprobe, vec3(f_lightmap.xy, float(slot * 10 + 1)));
        lightmap_intensity =
            clamp(((a.rgb + b.rgb) * 2.0 - 2.0) * (a.a * b.a * lightprobe_scales[slot].x), 0.0, 1.0);
    }

    vec3  to_eye   = camera_position - f_world_pos;
    float surface  = length(to_eye);
    vec3  incident = to_eye / max(surface, 1e-4);

    vec3 color = pow(max(texture(water_color, f_texcoord).rgb, vec3(0.0)), vec3(water_curves.x)) * shading.x;
    color *= lightmap_intensity * exposure;

    /* Refraction: the scene copy, shifted by the slope where it is behind
     * the water, fogged by how much water it is seen through */
    vec2  screen     = gl_FragCoord.xy / viewport;
    vec4  behind     = texture(scene_copy, screen);
    float depth_here = max(behind.a - surface, 0.0);
    vec2  bump = slope_refraction * incident.yx * refraction.x * clamp(3.0 * depth_here, 0.0, 1.0);
    bump *= min(max(2.0 / surface, 0.0), 1.0);
    vec2 refracted_uv = clamp(screen + bump, vec2(0.001), vec2(0.999));
    vec4 refracted    = texture(scene_copy, refracted_uv);
    if(refracted.a < surface)
        refracted = behind;
    vec3  bed      = refracted.rgb;
    vec3  under    = camera_position - incident * refracted.a;
    float by_depth = abs(under.z - f_world_pos.z);
    float by_dist  = max(refracted.a - surface, 0.0);
    float depth    = mix(by_dist, by_depth, refraction.z);
    float transparence = clamp(1.0 - fog_factor(shading.w, depth), 0.0, 1.0);
    transparence *= clamp(1.0 - surface / refraction.y, 0.0, 1.0);
    vec3  color_refraction = mix(color, bed, transparence);
    float bed_share        = transparence;

    /* Static reflection: the cube's alpha splits sunspot from sky */
    vec3 reflection = vec3(0.0);
    if(misc.w > 0.5)
    {
        vec3 r = reflect(-incident, normal);
        r.z    = abs(r.z);
        vec4  env   = texture(water_reflection, r);
        vec2  parts = vec2(clamp(env.a - misc.x, 0.0, 1.0), min(env.a, misc.x));
        vec3  sun_rate  = clamp(lightmap_intensity - misc.y, 0.0, 1.0);
        float sun_scale = dot(sun_rate, sun_rate);
        reflection = pow(max(env.rgb, vec3(0.0)), vec3(water_curves.y)) * (parts.x * sun_scale + parts.y) *
                     shading.y * exposure;
    }

    float fresnel = shading.z + (1.0 - shading.z) * pow(1.0 - clamp(dot(incident, normal), 0.0, 1.0), 2.5);
    vec3  result  = mix(color_refraction, reflection, fresnel);
    bed_share *= 1.0 - fresnel;
    result += water_diffuse.rgb * clamp(normal.z, 0.0, 1.0) * lightmap_intensity * exposure;

    /* Bank alpha: shallow water fades to the bed */
    float alpha = clamp(by_depth / max(refraction.w, 1e-3), 0.0, 1.0);
    result      = mix(bed, result, alpha);
    bed_share   = (1.0 - alpha) + bed_share * alpha;

    /* Foam over it all, lit like the diffuse */
    float covered = foam.z > 0.0 ? foam_factor(uv_main, uv_aux) : 0.0;
    if(covered > 0.002)
    {
        vec4 f = texture(foam_texture, f_texcoord * foam_xform.xy + foam_xform.zw) *
                 texture(foam_detail, f_texcoord * foam_detail_xform.xy + foam_detail_xform.zw);
        covered *= f.a;
        vec3 foam_color = pow(max(f.rgb, vec3(0.0)), vec3(2.0)) * lightmap_intensity * exposure;
        result    = mix(result, foam_color, covered);
        bed_share *= 1.0 - covered;
    }

    /* Fog applies to the water's own light, not the bed seen through it */
    vec3 extinction, inscatter;
    compute_scattering(camera_position, f_world_pos, extinction, inscatter);
    result = (result - bed * bed_share) * extinction + inscatter * exposure * (1.0 - bed_share) +
             bed * bed_share;
    out_color = vec4(max(result, vec3(0.0)), 1.0);
}
