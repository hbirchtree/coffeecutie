#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in uvec2 node_indices; /* bone indices (node0, node1) */
layout(location = 6) in vec2  node_weights; /* blend weights (weight0, weight1) */

struct InstanceData {
    mat4 transform;
    int  bone_base;
    int  _pad0;
    int  _pad1;
    int  _pad2;
};

layout(binding = 0, std140) uniform MatrixStore
{
    InstanceData data[128];
} matrices;

layout(location = 1) uniform mat4 camera;

layout(binding = 3, std430) readonly buffer BoneMatrices
{
    mat4 bones[];
} bone_store;

layout(location = 0) out FragData {
    vec3 position;
    vec2 tex;
    vec3 tangent;
    vec3 binormal;
    vec3 normal;
    vec2 light_tex;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    mat4 transform = matrices.data[gl_InstanceID].transform;
    int  bone_base = matrices.data[gl_InstanceID].bone_base;

    vec4 local_pos    = vec4(position, 1.0);
    vec4 local_normal = vec4(normal, 0.0);
    vec4 local_tan    = vec4(tangent, 0.0);
    vec4 local_bin    = vec4(binormal, 0.0);

    if(bone_base >= 0)
    {
        mat4 b0 = bone_store.bones[bone_base + int(node_indices.x)];
        mat4 b1 = bone_store.bones[bone_base + int(node_indices.y)];
        mat4 skin = b0 * node_weights.x + b1 * node_weights.y;
        local_pos    = skin * local_pos;
        local_normal = skin * local_normal;
        local_tan    = skin * local_tan;
        local_bin    = skin * local_bin;
    }

    vec4 world_pos = transform * local_pos;
    frag.tex        = tex;
    frag.instanceId = gl_InstanceID;
    frag.tangent    = (transform * local_tan).xyz;
    frag.binormal   = (transform * local_bin).xyz;
    frag.normal     = (transform * local_normal).xyz;
    frag.position   = world_pos.xyz;
    gl_Position     = camera * world_pos;
}
