#version 300 es
//#extension GL_ARB_separate_shader_objects : enable

precision highp float;
precision highp int;

layout(location=0)in vec3 pos;
layout(location=1)in vec2 tex;

#ifndef GL_ARB_separate_shader_objects
out vec2 vs_tc;
flat out int vs_instance;
#else
out VS_OUT
{
    vec2 vs_tc;
    flat int vs_instance;
} vs_out;
#endif

uniform mat4 transform[128];
uniform vec2 tex_mul[2];

void main(void)
{
    const vec2 tex_mul[2] = vec2[](vec2(1.,1.),vec2(-1.,1.));

    /* The current eye being rendered */
    int instance = gl_InstanceID;
    int eyeId = instance % 2;

    /* Regular vertex properties */
#ifndef GL_ARB_separate_shader_objects
    vs_instance = instance;
    vs_tc = vec2(1.0) - tex * tex_mul[eyeId];
#else
    vs_out.vs_instance = instance;
    vs_out.vs_tc = vec2(1.0) - tex * tex_mul[eyeId];
#endif
    mat4 xf = transform[instance];
    gl_Position = xf * vec4(pos,1.);
}
