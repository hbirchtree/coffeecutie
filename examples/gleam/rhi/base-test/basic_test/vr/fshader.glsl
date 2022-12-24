#version 410 core
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
    vec4 c1,c2,c3;

    if(fs_in.instance < 2)
    {
        c1 = texture(texdata,vec3(fs_in.tc,0)); /* Normal color */
        c2 = texture(texdata, vec3(fs_in.tc, 1.0)); /* Weird color */
    }else{
        c1 = c2 = texture(texdata,vec3(fs_in.tc,3));
    }

    if(c1.a==0)
        discard;

    float a1 = a1_mask.a;
    if(mx<a1)
        color = vec4(c1.rgb, 1.0);
    else
        color = vec4(c2.rgb, 1.0);
}
