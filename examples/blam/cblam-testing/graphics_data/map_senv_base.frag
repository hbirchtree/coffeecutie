#version 460 core

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

in FragData {
    vec3 world_pos;
    vec2 tex;
    vec2 light_tex;
    flat int instanceId;
} frag;

struct Map
{
    vec2 atlas_scale;
    vec2 atlas_offset;
    vec2 uv_scale;

    uint layer;
    float bias;
};

struct Lightmap
{
    vec2 atlas_scale;
    vec2 atlas_offset;
    uint layer;
};

struct Material
{
    Map      base;
    Lightmap lightmap;
};

uniform sampler2DArray base;
uniform sampler2DArray micro;
uniform sampler2DArray lightmaps;

uniform float render_distance;

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

out vec4 out_color;

void main()
{
//    if(length(frag.world_pos - vec3(0)) > render_distance)
//        discard;

    vec2 tex_ = frag.tex * mats.instance[frag.instanceId].base.uv_scale;
    tex_ = tex_ - floor(tex_);
    vec2 utex_ = frag.tex * mats.instance[frag.instanceId].micro.uv_scale;
    utex_ = utex_ - floor(utex_);

    vec2 b_a_scale = mats.instance[frag.instanceId].base.atlas_scale;
    vec2 m_a_scale = mats.instance[frag.instanceId].micro.atlas_scale;
    vec2 b_a_offset = mats.instance[frag.instanceId].base.atlas_offset;
    vec2 m_a_offset = mats.instance[frag.instanceId].micro.atlas_offset;
    uint layer = mats.instance[frag.instanceId].base.layer;
    uint m_layer = mats.instance[frag.instanceId].micro.layer;

    vec2 light_scale = mats.instance[frag.instanceId].lightmap.atlas_scale;
    vec2 light_offset = mats.instance[frag.instanceId].lightmap.atlas_offset;
    uint light_layer = mats.instance[frag.instanceId].lightmap.layer;

    vec2 sample_pos = (tex_ * b_a_scale + b_a_offset);
    vec2 usample_pos = (utex_ * m_a_scale + m_a_offset);

    vec4 color = texture(base, vec3(sample_pos, layer));
    vec4 micro_col = texture(micro, vec3(usample_pos, m_layer));
    vec4 lightmap_col =
            texture(lightmaps, vec3(
                        frag.light_tex * light_scale + light_offset,
                        light_layer));

    out_color.rgb = color.rgb * lightmap_col.rgb;
    out_color.a = 1;

//    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 0.2));
}
