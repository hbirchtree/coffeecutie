#version 100

#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying vec2  in_tex;
varying float in_shade;

uniform float time;
uniform float strength;
uniform float bands;

float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

/* Value noise wrapping every `period` cells along x; u wraps at 1 */
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

/* Broken wisps in rows up the cone: each row wobbles, tapers and is cut
 * into pieces of random length chasing round it at `speed` cells/s */
float wisps(vec2 uv, float rows, float cells, float speed)
{
    float v   = uv.y * rows;
    float row = floor(v);
    float x   = uv.x * cells - time * speed * (0.7 + 0.6 * hash(vec2(row, 3.0)));

    float wobble = (noise(vec2(x * 0.5, row * 5.0), cells * 0.5) - 0.5) * 0.7;
    float across = abs(fract(v + wobble) - 0.5) * 2.0;
    float body   = noise(vec2(x, row * 7.0 + 1.0), cells);
    float grow   = smoothstep(0.55, 0.9, body);
    float width  = 0.22 * grow;
    /* A firm core with a soft falloff and a faint halo, ends fading out */
    float core = 1.0 - smoothstep(width * 0.3, width, across);
    float halo = 1.0 - smoothstep(0.0, width * 2.5, across);
    return max(core, halo * 0.3) * smoothstep(0.0, 0.6, grow);
}

void main()
{
    float coarse = wisps(in_tex, bands, 6.0, 4.0);
    float fine   = wisps(in_tex + vec2(0.37, 0.0), bands * 2.3, 16.0, 9.0);
    float wind   = max(coarse, fine * 0.6);

    /* The painted fade thins the cone near the rifle, not out entirely */
    float shade = 0.35 + 0.65 * (1.0 - in_shade);
    float edge  = smoothstep(1.0, 0.85, in_tex.y);
    float alpha = clamp(0.75 * wind * shade * edge * strength, 0.0, 1.0);
    gl_FragColor = vec4(vec3(0.85, 0.38, 0.38) * alpha, alpha);
}
