#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location=0) in vec2 ndc;

out gl_PerVertex{
    vec4 gl_Position;
};

void main()
{
    gl_Position.xy = ndc;
    gl_Position.zw = vec2(0.0,1.0);
}
