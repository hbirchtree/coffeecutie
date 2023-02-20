#version 410 core
#define BLOCK_SIZE 512

precision highp float;
precision highp int;
precision highp sampler2DArray;

in vec3 position;
in vec2 tex;
in vec3 normal;
in vec3 binormal;
in vec3 tangent;

layout(std140) uniform MatrixStore
{
    mat4 transform[BLOCK_SIZE];
} matrices;

uniform mat4 camera;

out vec2 frag_tex;
flat out int frag_instanceId;

void main()
{
    frag_tex = tex;
    frag_instanceId = gl_InstanceID;
    gl_Position =
            camera * matrices.transform[gl_InstanceID] *
            vec4(position.xyz, 1);
}
