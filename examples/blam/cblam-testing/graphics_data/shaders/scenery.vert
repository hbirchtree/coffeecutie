#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;

layout(binding = 0, std140) uniform MatrixStore
{
    mat4 transform[128];
} matrices;

layout(location = 1) uniform mat4 camera;
//layout(location = 2) uniform mat3 cameraRotation;
//layout(location = 3) uniform vec3 camera_position;

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
    mat4 transform = matrices.transform[gl_InstanceID];
    mat3 rotation = mat3(transform);
    vec4 world_pos = transform * vec4(position, 1);
    frag.tex = tex;
    frag.instanceId = gl_InstanceID;
    frag.tangent = (transform * vec4(tangent, 0.0)).xyz;
    frag.binormal = (transform * vec4(binormal, 0.0)).xyz;
    frag.normal = (transform * vec4(normal, 0.0)).xyz;
    frag.position = world_pos.xyz;
    gl_Position = camera * world_pos;
}
