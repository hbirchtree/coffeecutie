#version 320 es

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
layout(binding = 1, std140) uniform Stuff
{
    float multiplier;
};

layout(location = 0) uniform mat4 camera;
layout(location = 1) uniform int g_BaseInstance;

layout(location=1) out gl_PerVertex
{
    vec4 gl_Position;
};

layout(location=0) out FragData
{
    out vec2 tex;
    flat out int instanceId;
} frag;

void main()
{
    float something = multiplier;
    something = something * 200.0;

    frag.tex = tex * multiplier;
    frag.instanceId = g_BaseInstance + gl_InstanceID;
    gl_Position =
            camera * matrices.transform[g_BaseInstance + gl_InstanceID] *
            vec4(position.xyz, 1);
}
