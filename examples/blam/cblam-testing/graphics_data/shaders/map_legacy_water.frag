#version 100
precision mediump float;

/* Simple ES2 water (swat). Halo water maps (base + ripple) are tangent-space
 * normal maps, not colour — there is no diffuse texture. So: a flat water
 * tint, a moving specular highlight derived from the ripple normal, and
 * lightmap modulation. No reflection cube / fresnel. */
varying vec2 frag_tex;
varying vec2 frag_light_tex;

uniform sampler2D base;   /* water normal (bound for parity; sampled for ripple) */
uniform sampler2D ripple;
uniform sampler2D lightmap;
uniform float     time;
uniform int       has_ripple;
uniform int       has_light;
uniform vec2      light_scale;
uniform vec2      light_offset;

void main()
{
    vec3  c     = vec3(0.05, 0.22, 0.32); /* deep water tint */
    float crest = 0.0;

    if(has_ripple > 0)
    {
        /* Two scrolling samples of the ripple normal for animated chop */
        vec3 n1 = texture2D(ripple, frag_tex * 2.0 + time * 0.02).rgb;
        vec3 n2 = texture2D(base, frag_tex * 3.0 - time * 0.015).rgb;
        vec3 n  = normalize((n1 + n2) - 1.0);
        crest   = pow(max(n.z, 0.0), 4.0);
        c += crest * vec3(0.25, 0.35, 0.45);
    }

    if(has_light > 0)
    {
        vec3 lm =
            texture2D(lightmap, frag_light_tex * light_scale + light_offset)
                .rgb;
        c *= lm * 1.8;
    }

    /* Semi-transparent; specular crests read a touch more opaque. */
    gl_FragColor = vec4(c, clamp(0.62 + crest, 0.0, 1.0));
}
