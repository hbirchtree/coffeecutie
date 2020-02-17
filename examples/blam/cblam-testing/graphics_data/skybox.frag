#version 320 es

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp samplerCubeArray;

in FragData {
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

uniform samplerCubeArray base;

void main()
{

}
