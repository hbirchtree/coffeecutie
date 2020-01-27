#version 320 es

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

in FragData {
    vec2 tex;
    flat int instanceId;
} frag;

struct Material
{
    vec2 scaling;
    vec2 uvscale;
    vec2 offset;
    int source;
    uint layer;
};

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

uniform sampler2DArray bc1_tex;
uniform sampler2DArray bc3_tex;
uniform sampler2DArray bc5_tex;

out vec4 out_color;

void main()
{
    vec2 tex_ = frag.tex;
    tex_ = tex_ - floor(tex_);

    vec2 sample_pos = tex_ * mats.instance[frag.instanceId].scaling
            + vec2(mats.instance[frag.instanceId].offset);
    uint layer = mats.instance[frag.instanceId].layer;

    if(mats.instance[frag.instanceId].source == 0)
        out_color = texture(bc1_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 1)
        out_color = texture(bc3_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 2)
        out_color = texture(bc5_tex, vec3(sample_pos, layer));

    if(out_color.a < 0.5)
        discard;
}
