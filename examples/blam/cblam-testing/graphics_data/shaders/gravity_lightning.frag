#version 100

precision highp float;

varying vec2 in_tex;

uniform sampler2D bolt;
uniform float     strength;

/* Drawn additively, black is clear */
void main()
{
    gl_FragColor = vec4(texture2D(bolt, in_tex).rgb * strength, 1.0);
}
