#version 460 core

layout(location = 0) in FragData {
    vec3 tbn_direction;
    vec3 eye_direction;
    vec3 position;
    vec2 tex;
    mat3 tbn;
    vec2 light_tex;
    flat int instanceId;
} frag;

layout(location = 15) uniform float render_distance;

layout(location = 0) out vec4 out_color;

void main()
{
//    float fade_in = abs((dist - render_distance + 10.0) / 10.0);
//    float fade_out = 1.0 - abs((dist - render_distance) / 10.0);

    out_color = vec4(0.0, 0.2, 1.0, 1);
}
