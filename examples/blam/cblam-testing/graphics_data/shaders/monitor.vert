#version 100

precision highp float;

attribute vec3 pos;
attribute vec3 normal;

varying vec3 in_normal;
varying vec3 in_view_dir;

uniform mat4 camera;
uniform mat4 model;
uniform vec3 camera_position;

void main()
{
    vec4 world_pos = model * vec4(pos, 1.0);
    /* Normals arrive as unorm u16; the model matrix is rotation and uniform
     * scale only, so it can transform them directly */
    in_normal   = (model * vec4(normal * 2.0 - 1.0, 0.0)).xyz;
    in_view_dir = camera_position - world_pos.xyz;
    gl_Position = camera * world_pos;
}
