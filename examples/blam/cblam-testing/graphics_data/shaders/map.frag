#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "fragments/map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 position;
    vec2 tex;
    mat3 tbn;
    vec2 light_tex;
    flat int instanceId;
} frag;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = vec4(frag.tex.xyy, 1.0);
}
