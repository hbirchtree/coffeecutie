/* Receiver side of render/cascaded_shadows.h. Includers may move the
 * uniforms with CSM_LOCATION (28 locations), CSM_BINDING,
 * CSM_OCCLUDER_BINDING and CSM_OCCLUDER_TILES_BINDING.
 *
 * Casters and static geometry give the light's dynamic and static
 * visibility, d and s; a cast shadow takes away s * (1 - d), the light the
 * static scene lets through and the casters block, as min(s, d) would
 * have it. Shade the static scene already gives is the baked lighting's,
 * which is soft, so s is softened to match while d stays sharp. */
#ifndef CSM_LOCATION
#define CSM_LOCATION 56
#endif
#ifndef CSM_BINDING
#define CSM_BINDING 9
#endif
#ifndef CSM_OCCLUDER_BINDING
#define CSM_OCCLUDER_BINDING (CSM_BINDING + 1)
#endif
#ifndef CSM_OCCLUDER_TILES_BINDING
#define CSM_OCCLUDER_TILES_BINDING (CSM_BINDING + 2)
#endif

layout(location=CSM_LOCATION) uniform mat4 csm_matrices[4];
/* World size of a texel, per cascade */
layout(location=CSM_LOCATION + 16) uniform vec4 csm_texel;
/* Cascades, strength, fade distance, how many (the first) have occluder
 * tiles; the rest use the world occluder map */
layout(location=CSM_LOCATION + 17) uniform vec4 csm_params;
/* Each cascade's rect of the atlas, inset for PCF */
layout(location=CSM_LOCATION + 18) uniform vec4 csm_tiles[4];
/* A texel's world size as depth, per cascade */
layout(location=CSM_LOCATION + 22) uniform vec4 csm_bias;
/* The static world from the light: world to uv + depth; its texel size
 * (0 = no world map) and depth per world unit, how far towards the light
 * static geometry counts and the world radius it is softened over */
layout(location=CSM_LOCATION + 23) uniform mat4 csm_occluder;
layout(location=CSM_LOCATION + 27) uniform vec4 csm_occluder_info;
layout(binding=CSM_BINDING) uniform sampler2D csm_atlas;
layout(binding=CSM_OCCLUDER_BINDING) uniform sampler2D csm_occluder_atlas;
/* The static world per cascade, in the same tiles as the casters */
layout(binding=CSM_OCCLUDER_TILES_BINDING) uniform sampler2D csm_occluder_tiles;

/* How steeply a surface facing the light by `facing` tilts away from it */
float light_slope(float facing)
{
    return min(sqrt(max(1.0 - facing * facing, 0.0)) / max(facing, 0.05), 4.0);
}

/* Static visibility over a disc of csm_occluder_info.w world units around
 * the receiver, so a cast shadow fades where it meets the static scene's
 * shade about as softly as the baked lighting draws it. Taps further out
 * allow for the surface sloping away from the light. */
float soft_visibility(
    sampler2D map,
    vec2      uv,
    float     z,
    vec4      rect,
    vec2      uv_per_world,
    float     depth_per_world,
    float     bias,
    float     slope)
{
    float radius = csm_occluder_info.w;
    float reach  = csm_occluder_info.z * depth_per_world;
    float lit    = 0.0;
    for(int y = -2; y <= 2; y++)
        for(int x = -2; x <= 2; x++)
        {
            vec2  o     = vec2(x, y) * 0.5 * radius;
            vec2  at    = clamp(uv + o * uv_per_world, rect.xy, rect.zw);
            float tilt  = length(o) * slope * depth_per_world;
            float wall  = texture(map, at).r;
            /* Geometry beyond the reach leaves its shade to the baked light */
            lit += max(step(z - bias - tilt, wall), step(wall, z - reach));
        }
    return lit * (1.0 / 25.0);
}

/* Static visibility from the world occluder map, for cascades without
 * occluder tiles: 1 = the light reaches `world` */
float world_visibility(vec3 world, vec3 n, float slope)
{
    if(csm_occluder_info.x <= 0.0)
        return 1.0;
    vec4 p = csm_occluder * vec4(world + n * csm_occluder_info.x, 1.0);
    if(any(lessThan(p.xy, vec2(0.0))) || any(greaterThan(p.xy, vec2(1.0))) || p.z >= 1.0)
        return 1.0;
    vec2 texel = 1.0 / vec2(textureSize(csm_occluder_atlas, 0));
    return soft_visibility(
        csm_occluder_atlas,
        p.xy,
        p.z,
        vec4(0.0, 0.0, 1.0, 1.0),
        texel / csm_occluder_info.x,
        csm_occluder_info.y,
        csm_occluder_info.x * csm_occluder_info.y * (1.0 + slope),
        slope);
}

/* How much of the light along `to_light` the casters take from `world`:
 * 0 = none. The casters' shadow is sharp; the static scene's visibility
 * that gates it is soft. */
float cascade_occlusion(vec3 world, vec3 n, vec3 to_light, vec3 eye)
{
    int   count  = int(csm_params.x);
    float facing = dot(n, to_light);
    float dist   = distance(world, eye);
    if(count == 0 || facing <= 0.0 || dist >= csm_params.z)
        return 0.0;
    vec2  texel = 1.0 / vec2(textureSize(csm_atlas, 0));
    float slope = light_slope(facing);
    for(int i = 0; i < count; i++)
    {
        /* Normal offset keeps lit faces off their own depth */
        vec4 p = csm_matrices[i] * vec4(world + n * csm_texel[i], 1.0);
        if(any(lessThan(p.xy, csm_tiles[i].xy)) ||
           any(greaterThan(p.xy, csm_tiles[i].zw)) || p.z >= 1.0)
            continue;
        float bias   = csm_bias[i] * (1.0 + slope);
        float hidden = 0.0;
        for(int y = -1; y <= 1; y++)
            for(int x = -1; x <= 1; x++)
                hidden += step(texture(csm_atlas, p.xy + vec2(x, y) * texel).r, p.z - bias);
        hidden *= 1.0 / 9.0;
        if(hidden <= 0.0)
            return 0.0;
        hidden *= i < int(csm_params.w + 0.5)
                      ? soft_visibility(
                            csm_occluder_tiles,
                            p.xy,
                            p.z,
                            csm_tiles[i],
                            texel / csm_texel[i],
                            csm_bias[i] / csm_texel[i],
                            bias,
                            slope)
                      : world_visibility(world, n, slope);
        float fade = 1.0 - smoothstep(csm_params.z * 0.8, csm_params.z, dist);
        return hidden * fade * smoothstep(0.0, 0.2, facing);
    }
    return 0.0;
}

/* Light reaching `world` at the configured strength: 1 = all */
float cascade_shadow(vec3 world, vec3 n, vec3 to_light, vec3 eye)
{
    return 1.0 - csm_params.y * cascade_occlusion(world, n, to_light, eye);
}
