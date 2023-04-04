#version 460 core

layout(location = 0) in FragData {
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
    float bias;
//    uint pad;
};

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

layout(location = 10, binding = 0) uniform sampler2DArray bc1_tex;
layout(location = 11, binding = 1) uniform sampler2DArray bc3_tex;
layout(location = 12, binding = 2) uniform sampler2DArray bc5_tex;

layout(location = 0) out vec4 out_color;

void main()
{
    vec2 tex_ = frag.tex * mats.instance[frag.instanceId].uvscale;
    tex_ = tex_ - floor(tex_);

    vec2 sample_pos = tex_ * mats.instance[frag.instanceId].scaling
            + vec2(mats.instance[frag.instanceId].offset);
    uint layer = mats.instance[frag.instanceId].layer;

    float bias = mats.instance[frag.instanceId].bias;

    if(mats.instance[frag.instanceId].source == 0)
        out_color = texture(bc1_tex, vec3(sample_pos, layer), bias);
    else if(mats.instance[frag.instanceId].source == 1)
        out_color = texture(bc3_tex, vec3(sample_pos, layer), bias);
    else if(mats.instance[frag.instanceId].source == 2)
        out_color = texture(bc5_tex, vec3(sample_pos, layer), bias);

//    out_color.rgb *= 0.01;
//    out_color.rgb += vec3(frag.tex.xy, float(layer) / 30.0);

//    out_color = vec4(frag.tex, 0.0, 1.0);

    if(out_color.a < 0.5)
        discard;

//    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 0.4));
}
