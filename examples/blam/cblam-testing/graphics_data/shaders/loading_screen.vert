#version 100

precision highp float;

attribute vec2 pos;
attribute vec2 tex;

varying vec2 in_tex;
varying vec2 in_pos;

uniform mat4 transform;
uniform vec2 tex_offset;

void main()
{
    in_pos = pos.xy;
    in_tex = tex - (tex_offset / 4.0);
    gl_Position = transform * vec4(pos.x, pos.y, 0.3, 1.0);
}
