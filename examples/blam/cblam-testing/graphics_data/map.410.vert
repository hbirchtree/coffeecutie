#version 410 core

precision highp float;
precision highp int;
precision highp sampler2DArray;

in vec3 position;
in vec2 tex;
in vec2 light_tex;

uniform mat4 camera;

out vec3 frag_world_pos;
out vec2 frag_tex;
out vec2 frag_light_tex;
flat out int frag_instanceId;

void main()
{
    frag_tex = tex;
    frag_light_tex = light_tex;
    frag_instanceId = gl_InstanceID;
    gl_Position = camera * vec4(position.xyz, 1);
    frag_world_pos = position;
}
