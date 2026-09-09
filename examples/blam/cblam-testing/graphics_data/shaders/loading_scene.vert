#version 100

precision mediump float;

attribute vec3 pos;
attribute vec2 tex;

varying vec2 in_tex;

uniform mat4 transform;

void main()
{
    in_tex = tex;
    gl_Position = transform * vec4(pos, 1.0);
}
