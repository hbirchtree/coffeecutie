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

float hash(float n)
{
    return fract(sin(n * 91.3458) * 47453.5453);
}

void main()
{
    /* Bands up the cone, each a few dashes chasing round it; u wraps at 1 */
    float v      = in_tex.y * bands;
    float id     = floor(v);
    float h      = hash(id + 1.0);
    float across = abs(fract(v) - 0.5) * 2.0;
    float line   = smoothstep(0.6 + 0.3 * h, 0.0, across);

    float dashes = 1.0 + floor(h * 3.0);
    float s      = fract(in_tex.x * dashes - time * (0.15 + 0.25 * h) + h * 7.0);
    float dash   = smoothstep(0.0, 0.25, s) * smoothstep(0.85, 0.3, s);

    /* The painted fade thins the cone near the rifle, not out entirely */
    float shade = 0.35 + 0.65 * (1.0 - in_shade);
    float edge  = smoothstep(1.0, 0.85, in_tex.y);
    float alpha = clamp(0.5 * line * dash * shade * edge * strength, 0.0, 1.0);
    gl_FragColor = vec4(vec3(0.85, 0.42, 0.42) * alpha, alpha);
}
