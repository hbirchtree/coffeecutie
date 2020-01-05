#version 460 core

in FragData {
    vec2 tex;
    flat int instanceId;
} frag;

struct Material
{
    vec2 scaling;
    ivec2 offset;
    uint source;
    uint layer;
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
    vec2 tex_dims = vec2(1);

    if(mats.instance[frag.instanceId].source == 0)
        tex_dims = vec2(textureSize(bc1_tex, 0).xy);
    else if(mats.instance[frag.instanceId].source == 1)
        tex_dims = vec2(textureSize(bc3_tex, 0).xy);
    else if(mats.instance[frag.instanceId].source == 2)
        tex_dims = vec2(textureSize(bc5_tex, 0).xy);

    vec2 tex_ = frag.tex - floor(frag.tex);

    vec2 sample_pos = tex_ * mats.instance[frag.instanceId].scaling
            + mats.instance[frag.instanceId].offset / tex_dims.xy;

    uint layer = mats.instance[frag.instanceId].layer;

    if(mats.instance[frag.instanceId].source == 0)
        out_color = texture(bc1_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 1)
        out_color = texture(bc3_tex, vec3(sample_pos, layer));
    else if(mats.instance[frag.instanceId].source == 2)
        out_color = texture(bc5_tex, vec3(sample_pos, layer));
}
