#version 100

precision lowp float;

attribute vec3 position;

uniform mat4 camera;

void main()
{
    gl_Position = camera * vec4(position, 1.0);
}
