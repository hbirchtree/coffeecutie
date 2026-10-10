#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 color;

layout(location = 0) uniform mat4 camera;

layout(location = 0) out FragmentData
{
    vec2 tex_coord;
    vec4 color;
} frag;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position    = camera * vec4(position, 1.0);
    frag.tex_coord = tex_coord;
    frag.color     = color;
}
