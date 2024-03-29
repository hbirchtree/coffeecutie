#version 100
precision mediump float;

varying vec2 frag_tex;
varying vec3 frag_normal;
varying vec2 frag_light_tex;
varying float frag_instanceId;

void main()
{
    gl_FragColor = vec4(frag_normal, 1.0);
}
