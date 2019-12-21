#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

uniform mat4 transform;

out gl_PerVertex
{
    vec4 gl_Position;
};

out FragData
{
    vec2 tex;
} frag;

void main()
{
    frag.tex = tex;
    gl_Position = transform * vec4(position, 1);
}
