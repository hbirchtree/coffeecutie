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
};

struct Material
{
    Map base;
    Map micro;
};

uniform sampler2DArray base;
uniform sampler2DArray micro;

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

    vec2 sample_pos = (tex_ * b_a_scale + b_a_offset);
    vec2 usample_pos = (utex_ * m_a_scale + m_a_offset);

    vec4 color = texture(base, vec3(sample_pos, layer));
    vec4 micro_col = texture(micro, vec3(usample_pos, m_layer));

//    micro_col = micro_col * 0.001 + vec4(1);

//    out_color.rgb = vec3(float(layer) / 40.0, float(m_layer) / 40.0, float(frag.instanceId) / 30) + color.rgb * 0.00001 + micro_col.rgb * 0.00001;
//    out_color.rgb = vec3(vec2(usample_pos), 0) + color.rgb * micro_col.rgb * 0.00001;
    out_color.rgb = color.rgb * micro_col.rgb;
    out_color.a = 1;
//    out_color = vec4(color.rgb * micro.rgb, color.a);
}
