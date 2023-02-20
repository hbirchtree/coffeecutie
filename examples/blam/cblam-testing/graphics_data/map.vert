#version 460 core

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec2 light_tex;

layout(location = 0) uniform mat4 camera;
layout(location = 1) uniform int g_BaseInstance;

out FragData {
    vec3 world_pos;
    vec2 tex;
    vec3 normal;
    vec3 binormal;
    vec3 tangent;
    vec2 light_tex;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    frag.tex = tex;
    frag.normal = normal;
    frag.binormal = binormal;
    frag.tangent = tangent;
    frag.light_tex = light_tex;
    frag.instanceId = g_BaseInstance + gl_InstanceID;
    gl_Position = camera * vec4(position.xyz, 1);
    frag.world_pos = position;
}
