#version 100
precision mediump float;

varying vec2 frag_tex;
varying vec3 frag_normal;
varying vec2 frag_light_tex;
varying float frag_instanceId;

uniform sampler2D base_map;
uniform sampler2D lightmap;

void main()
{
    vec3 base_color = texture2D(base_map, frag_tex).rgb;
    gl_FragColor = vec4(base_color + frag_normal / 2.0, 1.0);
}
