#version 460 core

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

layout(location = 0) out FragData {
    vec3 world_pos;
    vec2 tex;
    vec3 normal;
    vec3 binormal;
    vec3 tangent;
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
    frag.instanceId = gl_InstanceID;
    mat4 transform = matrices.transform[gl_InstanceID];
    vec4 world_pos = transform * vec4(position.xyz, 1);
    frag.world_pos = world_pos.xyz;
    gl_Position = camera * world_pos;
}
