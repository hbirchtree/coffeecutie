#version 100

precision highp float;
precision highp sampler2D;

varying vec2 in_tex;

uniform sampler2D source;
uniform vec2 halfpixel;
uniform float blur_offset;

void main()
{
    vec2 o = halfpixel * blur_offset;
    vec4 sum = texture2D(source, in_tex) * 4.0;
    sum += texture2D(source, in_tex + vec2(-o.x, -o.y));
    sum += texture2D(source, in_tex + vec2( o.x, -o.y));
    sum += texture2D(source, in_tex + vec2(-o.x,  o.y));
    sum += texture2D(source, in_tex + vec2( o.x,  o.y));
    gl_FragColor = sum / 8.0;
}
