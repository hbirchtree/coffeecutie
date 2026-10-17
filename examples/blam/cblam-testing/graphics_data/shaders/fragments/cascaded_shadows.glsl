/* Receiver side of render/cascaded_shadows.h. Includers may move the
 * uniforms with CSM_LOCATION (18 locations) and CSM_BINDING. */
#ifndef CSM_LOCATION
#define CSM_LOCATION 56
#endif
#ifndef CSM_BINDING
#define CSM_BINDING 9
#endif

layout(location=CSM_LOCATION) uniform mat4 csm_matrices[4];
/* World size of a texel, per cascade */
layout(location=CSM_LOCATION + 16) uniform vec4 csm_texel;
/* Cascades, atlas texel width, strength, distance */
layout(location=CSM_LOCATION + 17) uniform vec4 csm_params;
layout(binding=CSM_BINDING) uniform sampler2D csm_atlas;

/* How much of the light along `to_light` reaches `world`: 1 = all */
float cascade_shadow(vec3 world, vec3 n, vec3 to_light, vec3 eye)
{
    int   count  = int(csm_params.x);
    float facing = dot(n, to_light);
    float dist   = distance(world, eye);
    if(count == 0 || facing <= 0.0 || dist >= csm_params.w)
        return 1.0;
    float tile   = 1.0 / float(count);
    vec2  texel  = vec2(csm_params.y, csm_params.y * float(count));
    for(int i = 0; i < count; i++)
    {
        /* Normal offset keeps lit faces off their own depth */
        vec4 p = csm_matrices[i] * vec4(world + n * csm_texel[i] * 1.5, 1.0);
        vec2 lo = vec2(tile * float(i), 0.0) + texel * 2.0;
        vec2 hi = vec2(tile * float(i + 1), 1.0) - texel * 2.0;
        if(any(lessThan(p.xy, lo)) || any(greaterThan(p.xy, hi)) || p.z >= 1.0)
            continue;
        float hidden = 0.0;
        for(int y = -1; y <= 1; y++)
            for(int x = -1; x <= 1; x++)
                hidden += step(
                    texture(csm_atlas, p.xy + vec2(x, y) * texel).r, p.z - 0.0002);
        hidden *= 1.0 / 9.0;
        float fade = 1.0 - smoothstep(csm_params.w * 0.8, csm_params.w, dist);
        return 1.0 - csm_params.z * hidden * fade * smoothstep(0.0, 0.2, facing);
    }
    return 1.0;
}
