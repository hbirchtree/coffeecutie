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

const uint TEX_BC1    = 1u;
const uint TEX_BC2    = 2u;
const uint TEX_BC3    = 3u;
const uint TEX_RGBA4  = 7u;
const uint TEX_RGBA8  = 8u;

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
    else if(source == TEX_RGBA4)
        return texture(source_rgba4, vec3(sample_coord, layer), bias).bgra;
    else if(source == TEX_RGBA8)
        return texture(source_rgba8, vec3(sample_coord, layer), bias).bgra;
    else
        return vec4(1.0, 0.0, 1.0, 0.0);
}

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 coloring = elements.instances[frag.element_id].color.rgb;
    vec4 background_color = sample_color();
    out_color = vec4(coloring.rgb * background_color.rgb, background_color.a);
}
