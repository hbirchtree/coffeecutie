#version 320 es

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

struct Material
{
    vec2 scaling;
    vec2 uvscale;
    vec2 offset;
    int source;
    uint layer;
    float bias;
};

uniform sampler2DArray bc1_tex;
uniform sampler2DArray bc3_tex;
uniform sampler2DArray bc5_tex;

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

out vec4 out_color;

void main()
{
    vec2 tex_ = frag.tex - floor(frag.tex);

    vec2 sample_pos = tex_ * mats.instance[frag.instanceId].scaling
            + vec2(mats.instance[frag.instanceId].offset);

    uint layer = mats.instance[frag.instanceId].layer;

    if(mats.instance[frag.instanceId].source == 0)
        out_color = texture(bc1_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 1)
        out_color = texture(bc3_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 2)
        out_color = texture(bc5_tex, vec3(sample_pos, layer));

    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 0.8));
}
