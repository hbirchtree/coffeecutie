#version 460 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 color;

layout(location=0) uniform mat4 camera;

layout(location=0) out vec3 frag_color;

void main()
{
    gl_Position = camera * vec4(position, 1.0);
    frag_color = color;
}
