#version 330 core
in VS_OUT{
    vec2 tc;
    flat int instance;
} fs_in;

layout(location = 0) out vec4 color;
uniform float mx;

uniform sampler2DArray texdata;

void main(void)
{
    /*Common alpha mask*/
    vec4 a1_mask = texture(texdata,vec3(fs_in.tc,2));

    /* Colors to blend */
    vec4 c1 = texture(texdata,vec3(fs_in.tc,0)); /* Normal color */
    vec4 c2 = texture(texdata,vec3(fs_in.tc,1)); /* Weird color */
    vec4 c3 = texture(texdata,vec4(fs_in.tc,2)); /* Veiny stuff */

    float a1 = a1_mask.a;
    if(mx>a1)
        color = c1;
    else
        color = c2;

    if((mx-1.0)>a1)
    {
        color+=c3;
    }
}