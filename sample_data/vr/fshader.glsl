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
    /* Blend the textures together by using an alpha mask as threshold */
    vec4 c1 = texture(texdata,vec3(fs_in.tc,0));
    vec4 c2 = texture(texdata,vec3(fs_in.tc,1));
    vec4 amask = texture(texdata,vec3(fs_in.tc,2));
    float a1 = amask.a;
    if(mx>a1)
        color = c1;
    else
        color = c2;
}