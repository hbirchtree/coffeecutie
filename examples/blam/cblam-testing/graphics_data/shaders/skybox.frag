#version 320 es

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp samplerCubeArray;

layout(location = 0) in FragData {
    vec2 tex;
    flat int instanceId;
} frag;

struct
{
    vec2 scaling;
    vec2 uvscale;
    vec2 offset;
    uint layer;
};

layout(location = 10) uniform samplerCubeArray base;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(texture(base, vec4(vec3(frag.tex, 0.0 ), 0.0)).rgb, 1.0);
}
