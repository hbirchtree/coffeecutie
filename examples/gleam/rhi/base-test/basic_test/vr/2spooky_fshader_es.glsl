#version 300 es
#extension GL_ARB_separate_shader_objects : enable

precision lowp float;

#ifndef GL_ARB_separate_shader_objects
in vec2 vs_tc;
flat in int vs_instance;
#else
in VS_OUT
{
    vec2 vs_tc;
    flat int vs_instance;
} fs_in;
#endif

layout(location = 0) out vec4 color;

uniform float mx;

uniform mediump sampler2DArray texdata;

void main(void)
{
#ifdef GL_ARB_separate_shader_objects
    vec2 vs_tc = vs_in.vs_tc;
    int vs_instance = vs_in.vs_instance;
#endif

    /*Common alpha mask*/
    vec4 a1_mask = texture(texdata,vec3(vs_tc,2));

    /* Colors to blend */
    vec4 c1,c2,c3;

    if(vs_instance < 2)
    {
        c1 = texture(texdata,vec3(vs_tc,0)); /* Normal color */
        c2 = texture(texdata,vec3(vs_tc,1)); /* Weird color */
        c3 = texture(texdata,vec3(vs_tc,3)); /* Veiny stuff */
    }else{
        c1 = c2 = c3 = texture(texdata,vec3(vs_tc,4));
    }

    if(c1.a==0.)
        discard;

    float a1 = a1_mask.a;
    if(mx<a1)
        color = c1;
    else
        color = c2;

    if((mx-0.5)>a1 && c3.a > 0.3)
    {
        color=c3;
    }
}
