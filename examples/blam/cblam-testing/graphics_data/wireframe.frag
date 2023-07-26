#version 460 core

layout(location = 0) in FragData {
    vec3 position;
    vec2 tex;
    mat3 tbn;
    vec2 light_tex;
    flat int instanceId;
} frag;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(0.0, 0.2, 1.0, 1);
}
