#version 460 core
layout(location=0) in vec2 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;

layout(location=0) out vec2 f_texcoord;
layout(location=1) out vec4 f_color;

/* Halo's 640x480 screen, centred, y up, into clip space */
layout(location=0) uniform vec2 screen_scale;

void main()
{
    f_texcoord  = texcoord;
    f_color     = color;
    gl_Position = vec4(position * screen_scale, 0.0, 1.0);
}
