#version 100
precision mediump float;

/* Simplified ES2 shader_chicago (schi/scex) combiner: the full register-
 * combiner chain is collapsed to base * detail (double-multiply, the common
 * chicago op) with any further layers added on top. Enough for the sky dome
 * (base gradient * star detail) and simple multi-map scenery, without the
 * per-stage flag machinery of the ES3 ubershader. */
varying vec2 frag_tex;

uniform sampler2D map0;
uniform sampler2D map1;
uniform sampler2D map2;
uniform sampler2D map3;
uniform int       map_count;
uniform vec2      base_map_scale;

void main()
{
    /* schi/scex maps are RGB565/RGBA8, stored BGR (the ES3 uber applies the
     * same .bgra swizzle for these formats). */
    vec3 c = texture2D(map0, frag_tex * base_map_scale).bgr;
    if(map_count > 1)
        c = clamp(c * texture2D(map1, frag_tex).bgr * 2.0, 0.0, 1.0);
    if(map_count > 2)
        c = clamp(c + texture2D(map2, frag_tex).bgr, 0.0, 1.0);
    if(map_count > 3)
        c = clamp(c + texture2D(map3, frag_tex).bgr, 0.0, 1.0);
    gl_FragColor = vec4(c, 1.0);
}
