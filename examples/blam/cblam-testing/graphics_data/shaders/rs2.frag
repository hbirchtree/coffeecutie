#version 460 core

layout(location=0) in vec4 f_color;
layout(location=1) in vec3 f_world_pos;
layout(location=2) in vec2 f_texcoord;
layout(location=3) flat in int f_instanceID;

layout(location=2) uniform sampler2DArray f_textures;
layout(location=3) uniform int f_texture_ids[256];

layout(location=0) out vec4 out_color;
void main()
{
    vec3 normal = normalize(cross(dFdx(f_world_pos), dFdy(f_world_pos)));
    vec3 sun = normalize(vec3(-0.35, -0.35, 20.87));
    float lightness = 0.3 + 0.25 * abs(dot(normal, sun));
    if(f_texture_ids[f_instanceID] < 0)
    {
        out_color = vec4(f_color.rgb * lightness, f_color.a);
    } else
    {
        vec4 color = texture(
            f_textures,
            vec3(f_texcoord, float(f_texture_ids[f_instanceID]))
        );
        if(color.a < 0.5)
            discard;
        out_color = color * lightness;
    }
}
