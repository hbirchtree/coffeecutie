#version 100

precision highp float;

varying vec2 in_tex;

uniform sampler2D color;

void main()
{
    gl_FragColor = vec4(texture2D(color, in_tex).rgb, 1.0);
}
