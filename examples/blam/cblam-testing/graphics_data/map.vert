#version 460 core

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex;
layout(location = 5) in vec2 light_tex;

uniform mat4 camera;

out FragData {
    vec3 world_pos;
    vec2 tex;
    vec2 light_tex;
    flat int instanceId;
} frag;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
    frag.tex = tex;
    frag.light_tex = light_tex;
    frag.instanceId = gl_BaseInstance + gl_InstanceID;
    gl_Position = camera * vec4(position, 1);
    frag.world_pos = position;
}
