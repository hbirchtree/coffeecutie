#version 330

layout (location = 1) in vec3 inPos;
layout (location = 3) in float inLife;

void main()
{
    gl_Position = vec4(inPos, 1.0);
}
