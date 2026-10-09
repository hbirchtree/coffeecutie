#version 100
precision mediump float;

/* Scaled UVs reach the hundreds; mediump is fp16 on SGX/Mali-4xx */
#ifdef GL_FRAGMENT_PRECISION_HIGH
#define TEXCOORD_PRECISION highp
#else
#define TEXCOORD_PRECISION mediump
#endif

varying TEXCOORD_PRECISION vec2 frag_base_tex;
varying TEXCOORD_PRECISION vec2 frag_micro_tex;
varying vec3 frag_normal;
varying vec2 frag_light_tex;
varying float frag_instanceId;

uniform sampler2D base_map;
uniform sampler2D micro_map;

uniform sampler2D lightmap;

void main()
{
    int instanceId = int(frag_instanceId);
    vec3 base_color = texture2D(base_map, frag_base_tex).rgb;
    vec3 micro = texture2D(micro_map, frag_micro_tex).rgb;
    vec3 light_color = texture2D(lightmap, frag_light_tex).rgb;

    gl_FragColor = vec4(base_color * micro * light_color, 1.0);
}
