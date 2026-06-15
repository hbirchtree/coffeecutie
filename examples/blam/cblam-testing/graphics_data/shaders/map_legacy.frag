#version 100
precision mediump float;

varying vec2 frag_tex;
varying vec3 frag_normal;
varying vec2 frag_light_tex;
varying float frag_instanceId;

uniform sampler2D base_map;
uniform vec2 base_map_scale;

uniform sampler2D micro_map;
uniform vec2 micro_map_scale;

uniform sampler2D lightmap;

void main()
{
    int instanceId = int(frag_instanceId);
    vec3 base_color = texture2D(
        base_map,
        frag_tex * base_map_scale).rgb;
    vec3 micro = texture2D(
        micro_map,
        frag_tex * micro_map_scale).rgb;
    vec3 light_color = texture2D(lightmap, frag_light_tex).rgb;

    gl_FragColor = vec4(base_color * micro * light_color, 1.0);
}
