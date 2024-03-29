#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coord;

layout(location = 0) uniform mat3 screen_scale;

layout(location = 0) out FragmentData
{
    vec2     tex_coord;
    flat int element_id;
} frag;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = vec4(vec3((screen_scale * vec3(position, 1.0)).xy, 0.0), 1.0);
    frag.tex_coord  = tex_coord;
    frag.element_id = gl_VertexID / 6;
}
