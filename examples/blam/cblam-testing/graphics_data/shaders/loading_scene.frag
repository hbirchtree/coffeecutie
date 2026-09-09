#version 100

precision mediump float;
precision mediump sampler2D;

varying vec2 in_tex;

uniform sampler2D surface;

void main()
{
    gl_FragColor = vec4(
        dot(texture2D(surface, in_tex).rgb, vec3(0.2126, 0.7152, 0.0722)) *
        vec3(0.1, 0.475, 1.0),
        1.0);
}
