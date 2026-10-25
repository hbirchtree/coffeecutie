#version 100

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec3  in_local;
varying float in_shade;

uniform float time;
uniform float strength;

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

/* Value noise wrapping every `period` cells along x, so the swirl has no seam */
float noise(vec2 p, float period)
{
    vec2 i = floor(p);
    vec2 f = fract(p);
    f      = f * f * (3.0 - 2.0 * f);
    float x0 = mod(i.x, period);
    float x1 = mod(i.x + 1.0, period);
    return mix(
        mix(hash(vec2(x0, i.y)), hash(vec2(x1, i.y)), f.x),
        mix(hash(vec2(x0, i.y + 1.0)), hash(vec2(x1, i.y + 1.0)), f.x),
        f.y);
}

/* `shear` seconds of the inner cloud outrunning the outer */
float clouds(float r, float turn, float shear, float seed)
{
    /* Winds up toward the trumpet, turning faster there */
    float swirl = turn + time * 0.01 + 0.4 / (r + 0.3) + shear * 0.02 / (r + 0.5);
    float depth = log(r + 0.1) * 2.5 - time * 0.05 + seed;
    return 0.5 * noise(vec2(swirl * 8.0, depth), 8.0) +
           0.3 * noise(vec2(swirl * 16.0, depth * 2.0), 16.0) +
           0.2 * noise(vec2(swirl * 32.0, depth * 4.0), 32.0);
}

void main()
{
    float r    = length(in_local.xz);
    float turn = atan(in_local.z, in_local.x) / 6.2831853 + 0.5;

    /* Shear left to run would wind the cloud into one tight spiral: two
     * copies restart it half a period apart, each unseen as it does */
    float period = 20.0;
    float a      = fract(time / period);
    float b      = fract(time / period + 0.5);
    float cloud  = mix(clouds(r, turn, b * period, 17.0),
                       clouds(r, turn, a * period, 0.0),
                       1.0 - abs(2.0 * a - 1.0));

    /* Squared, so the slopes either side of the ring stay heavy */
    float dense = 1.0 - in_shade * in_shade;
    float alpha = clamp(dense * mix(0.75, 1.5, cloud) * strength, 0.0, 1.0);
    vec3  color = mix(vec3(0.24, 0.24, 0.26), vec3(0.6, 0.6, 0.61), cloud * cloud);
    gl_FragColor = vec4(color * alpha, alpha);
}
