#version 100

precision highp float;

varying vec2 in_pos;
varying vec2 in_tex;

uniform sampler2D noise;
uniform float curtain_fade;

void main()
{
    float fade = pow(length(abs(in_pos)), 1.2);
    gl_FragColor = vec4(
        vec3(
            length(texture2D(noise, in_tex).rgb * fade) *
            3.0 *
            texture2D(noise, in_tex * vec2(-0.2, 0.05)).rgb
        ),
        curtain_fade
    );
}
