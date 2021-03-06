#version 460 core

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;

layout(binding = 0, std140) buffer MatrixStore
{
    mat4 transform[];
} matrices;

uniform mat4 camera;

out gl_PerVertex
{
    vec4 gl_Position;
};

out FragData
{
    vec2 tex;
    flat int instanceId;
} frag;

void main()
{
    frag.tex = tex;
    frag.instanceId = gl_BaseInstance + gl_InstanceID;
    gl_Position =
            camera * matrices.transform[gl_BaseInstance + gl_InstanceID] *
            vec4(position.xyz, 1);
}
