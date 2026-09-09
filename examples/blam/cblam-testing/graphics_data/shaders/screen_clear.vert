#version 100

precision highp float;

attribute vec2 pos;
attribute vec2 tex;

varying vec2 in_tex;

uniform mat4 transform;

void main()
{
    in_tex = tex;
    gl_Position = transform * vec4(pos.x, pos.y, 0.0, 1.0);
}
