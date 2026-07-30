#version 460 core

#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in uint normal;
layout(location = 3) in uint binormal;
layout(location = 4) in uint tangent;
layout(location = 5) in vec2 light_tex;

layout(location = 1) uniform mat4 camera;
layout(location = 2) uniform mat3 cameraRotation;
layout(location = 3) uniform vec3 camera_position;

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

#include "fragments/unpack_uvec3.glsl"

void main()
{
    frag.tex        = tex;
    frag.light_tex  = light_tex;
    frag.instanceId = gl_InstanceID;
    gl_Position = camera * vec4(position, 1);

    frag.tangent = unpack_uvec3(tangent);
    frag.binormal = unpack_uvec3(binormal);
    frag.normal = unpack_uvec3(normal);
    frag.position = position;
}
