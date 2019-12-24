#version 460 core

in FragData {
    vec2 tex;
    flat int instanceId;
} frag;

//struct Material
//{
//    vec2 scaling;
//    uint layer;
//};

//uniform sampler2DArray color_tex;
//layout(binding = 1) buffer MaterialProperties
//{
//    Material instance[];
//} materials;

out vec4 out_color;

void main()
{
//    vec4 color = texture(color_tex, vec3(frag.tex, 10));
//    out_color = vec4(color.rgb, 1);
    out_color = vec4(frag.tex.xyy, 1);
}
