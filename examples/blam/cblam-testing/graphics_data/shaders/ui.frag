#version 460 core

layout(location = 0) in FragmentData {
    vec2 tex_coord;
    flat int element_id;
} frag;

struct Instance
{
    vec4 color;
    vec4 tex_scale_offset;
    uvec4 texture_source; // x = the value
    uvec4 padding;
};

layout(binding = 0, std140) uniform InstanceData {
    Instance instances[512];
} elements;

layout(location = 10, binding = 0) uniform sampler2DArray source_bc1;
layout(location = 11, binding = 1) uniform sampler2DArray source_bc2;
layout(location = 12, binding = 2) uniform sampler2DArray source_bc3;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgba4;
layout(location = 14, binding = 4) uniform sampler2DArray source_rgba8;
layout(location = 15, binding = 5) uniform sampler2DArray source_font;
layout(location = 16, binding = 6) uniform sampler2DArray source_rg8;
layout(location = 17, binding = 7) uniform sampler2DArray source_etc2_rgb;
layout(location = 18, binding = 8) uniform sampler2DArray source_etc2_rgba;

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;
const uint TEX_FONT   = 9u;
const uint TEX_A8Y8   = 12u;
const uint TEX_ETC2_RGB  = 13u;
const uint TEX_ETC2_RGBA = 14u;

vec4 sample_color()
{
    vec4 scale_offset = elements.instances[frag.element_id].tex_scale_offset;
    vec2 sample_coord = frag.tex_coord * scale_offset.xy + scale_offset.zw;
    uint texture_source = elements.instances[frag.element_id].texture_source.x;
    uint source = texture_source >> 24;
    uint layer = texture_source & 0xFFFFFF;
    const float bias = -10.0;
    if(source == TEX_BC1)
        return texture(source_bc1, vec3(sample_coord, layer), bias);
    else if(source == TEX_BC2)
        return texture(source_bc2, vec3(sample_coord, layer), bias);
    else if(source == TEX_BC3)
        return texture(source_bc3, vec3(sample_coord, layer), bias);
    /* RGBA4 = D3DFMT_A4R4G4B4: GL reads (A,R,G,B) in bits[15:12..3:0],
     * giving texture.rgba=(Halo.A, Halo.R, Halo.G, Halo.B).
     * .gbar reorders to (Halo.R, Halo.G, Halo.B, Halo.A). */
    else if(source == TEX_RGBA4)
        return texture(source_rgba4, vec3(sample_coord, layer), bias).gbar;
    /* RGBA8 = D3DFMT_A8R8G8B8: bytes in memory are B,G,R,A (little-endian),
     * giving texture.rgba=(B, G, R, A). .bgra reorders to (R, G, B, A). */
    else if(source == TEX_RGBA8)
        return texture(source_rgba8, vec3(sample_coord, layer), bias).bgra;
    /* A8Y8: luminance in r, alpha in g; tinted by the instance colour */
    else if(source == TEX_A8Y8)
    {
        vec2 la = texture(source_rg8, vec3(sample_coord, layer), bias).rg;
        return vec4(la.rrr, la.g);
    }
    else if(source == TEX_ETC2_RGB)
        return texture(source_etc2_rgb, vec3(sample_coord, layer), bias);
    else if(source == TEX_ETC2_RGBA)
        return texture(source_etc2_rgba, vec3(sample_coord, layer), bias);
    else if(source == TEX_FONT)
    {
        float alpha = texture(source_font, vec3(sample_coord, layer)).r;
        vec4 col = elements.instances[frag.element_id].color;
        return vec4(col.rgb, alpha * col.a);
    }
    else
        return vec4(1.0, 0.0, 1.0, 0.0);
}

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 coloring = elements.instances[frag.element_id].color.rgb;
    vec4 background_color = sample_color();
    uint source = elements.instances[frag.element_id].texture_source.x >> 24;
    if(source == TEX_FONT)
        out_color = background_color;
    else
        out_color = vec4(coloring.rgb * background_color.rgb, background_color.a);
}
