#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;

layout(binding = 0, std140) uniform MatrixStore
{
    mat4 transform[256];
} matrices;

layout(location = 1) uniform mat4 camera;
//layout(location = 2) uniform mat3 cameraRotation;
//layout(location = 3) uniform vec3 camera_position;

layout(location = 0) out FragData {
    vec3 tbn_direction;
    vec3 eye_direction;
    vec3 position;
    vec2 tex;
    mat3 tbn;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    mat4 transform = matrices.transform[gl_InstanceID];
    mat3 rotation = transform;
    mat3 tbn = mat3(tangent, binormal, normal);
    vec4 world_pos = transform * vec4(position.xyz, 1);
    frag.tex = tex;
    frag.instanceId = gl_InstanceID;
    frag.tbn = tbn;
    frag.position = world_pos.xyz * -1;
    gl_Position = camera * world_pos;
//    frag.eye_direction = normalize(
//        tbn * normalize(camera_position - (world_pos.xyz * -1)).xyz
//        );
//    frag.tbn_direction = normalize(
//        tbn *
//        (
//            normalize(camera_position - (world_pos.xyz * -1)).xyz *
//            vec3(1, -1, -1))
//        );
}
