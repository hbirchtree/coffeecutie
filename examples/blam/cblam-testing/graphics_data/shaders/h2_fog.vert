#version 460 core
layout(location=0) in vec2 position;

layout(location=0) out vec2 f_ndc;

void main()
{
    f_ndc       = position;
    gl_Position = vec4(position, 0.0, 1.0);
}
