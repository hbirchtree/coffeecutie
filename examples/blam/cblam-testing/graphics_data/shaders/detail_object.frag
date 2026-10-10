#version 460 core

layout(location = 0) in FragmentData
{
    vec2 tex_coord;
    vec4 color;
} frag;

layout(location = 1) uniform int   texture_source; /* format << 24 | layer */
layout(location = 2) uniform float bias;           /* atlas mip alignment */
layout(location = 10, binding = 0) uniform sampler2DArray source;

layout(location = 0) out vec4 out_color;

const int TEX_RGBA4 = 7;
const int TEX_RGBA8 = 8;
const int TEX_A8Y8  = 12;

void main()
{
    int  format = texture_source >> 24;
    vec4 texel  = texture(
        source, vec3(frag.tex_coord, float(texture_source & 0xFFFFFF)), bias);
    /* Same channel orders as the UI's sampling */
    if(format == TEX_RGBA4)
        texel = texel.gbar;
    else if(format == TEX_RGBA8)
        texel = texel.bgra;
    else if(format == TEX_A8Y8)
        texel = texel.rrrg;
    if(texel.a < 0.5)
        discard;
    /* Baked lighting is stored at half intensity */
    out_color = vec4(texel.rgb * frag.color.rgb * 2.0, 1.0);
}
