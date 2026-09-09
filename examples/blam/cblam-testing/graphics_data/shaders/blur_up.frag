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
    vec4 sum = texture2D(source, in_tex + vec2(-o.x * 2.0, 0.0));
    sum += texture2D(source, in_tex + vec2(-o.x,  o.y)) * 2.0;
    sum += texture2D(source, in_tex + vec2( 0.0,  o.y * 2.0));
    sum += texture2D(source, in_tex + vec2( o.x,  o.y)) * 2.0;
    sum += texture2D(source, in_tex + vec2( o.x * 2.0, 0.0));
    sum += texture2D(source, in_tex + vec2( o.x, -o.y)) * 2.0;
    sum += texture2D(source, in_tex + vec2( 0.0, -o.y * 2.0));
    sum += texture2D(source, in_tex + vec2(-o.x, -o.y)) * 2.0;
    gl_FragColor = sum / 12.0;
}
