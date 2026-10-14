#version 460 core
layout(location=0) in vec2 f_texcoord;
layout(location=1) in vec4 f_color;

layout(location=0) out vec4 out_color;

layout(binding=0) uniform sampler2D image;

void main()
{
    out_color = texture(image, f_texcoord) * f_color;
}
