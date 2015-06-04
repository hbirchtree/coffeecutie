#version 330

layout (location = 1) in vec3 inPos;

void main()
{
    gl_Position = vec4(inPos, 1.0);
}
