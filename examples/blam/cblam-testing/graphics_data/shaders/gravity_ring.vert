#version 100

precision highp float;

attribute vec3 pos;
attribute vec2 tex;

varying vec2 in_tex;
varying vec3 in_world;

uniform mat4 camera;
uniform mat4 model;

void main()
{
    vec4 world  = model * vec4(pos, 1.0);
    in_tex      = tex;
    in_world    = world.xyz;
    gl_Position = camera * world;
}
