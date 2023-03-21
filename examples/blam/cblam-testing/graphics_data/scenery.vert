#version 460 core

precision highp float;
precision highp int;
precision highp sampler2DArray;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;

layout(binding = 0, std140) buffer MatrixStore
{
    mat4 transform[];
} matrices;

layout(location = 0) uniform mat4 camera;

out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location = 0) out FragData
{
    vec2 tex;
    flat int instanceId;
} frag;

void main()
{
    frag.tex = tex;
    frag.instanceId = gl_InstanceID;
    gl_Position =
            camera * matrices.transform[gl_InstanceID] *
            vec4(position.xyz, 1);
}
