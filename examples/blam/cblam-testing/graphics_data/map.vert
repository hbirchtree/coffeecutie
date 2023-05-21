#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 binormal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec2 light_tex;

layout(location = 1) uniform mat4 camera;
layout(location = 2) uniform mat3 cameraRotation;
layout(location = 3) uniform vec3 camera_position;

layout(location = 0) out FragData {
    vec3 position;
    vec2 tex;
    mat3 tbn;
    vec2 light_tex;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    frag.tex        = tex;
    frag.light_tex  = light_tex;
    frag.instanceId = gl_InstanceID;
    gl_Position = camera * vec4(position, 1);

    mat3 tbn = mat3(-tangent, -binormal, -normal);
    frag.tbn = tbn;
    frag.position = position * -1;
}
