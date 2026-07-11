#version 460 core
layout(location=0) in vec3 position;
layout(location=1) in vec2 texcoord;
layout(location=2) in vec4 color;

layout(location=0) out vec4 f_color;
layout(location=1) out vec3 f_world_pos;
layout(location=2) out vec2 f_texcoord;
layout(location=3) flat out int f_instanceID;

layout(location=0) uniform float world_scale;
layout(location=1) uniform mat4 camera;

void main()
{
    f_color = color;
    f_world_pos = position / world_scale;
    f_texcoord = texcoord;
    f_instanceID = gl_InstanceID;
    gl_Position = camera * vec4(position / world_scale, 1.0);
}
