#version 100

precision highp float;

attribute vec3 pos;
attribute vec2 tex;
attribute vec4 color;

varying vec3  in_local;
varying vec2  in_tex;
varying float in_shade;

uniform mat4 camera;
uniform mat4 model;

void main()
{
    in_local    = pos;
    in_tex      = tex;
    /* Painted in the mesh: black is dense, white is clear */
    in_shade    = color.r;
    gl_Position = camera * model * vec4(pos, 1.0);
}
