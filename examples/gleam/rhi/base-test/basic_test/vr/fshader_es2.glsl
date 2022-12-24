#version 100

precision lowp float;

varying vec2 vs_tc;

uniform mediump sampler2D texdata;

void main(void)
{
    gl_FragColor = texture2D(texdata, vs_tc / 2.);
}
