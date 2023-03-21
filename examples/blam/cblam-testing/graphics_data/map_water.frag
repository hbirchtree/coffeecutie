#version 460 core

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(vec3(0, 0, 1), .1);
}
