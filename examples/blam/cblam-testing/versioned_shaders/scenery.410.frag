#version 410 core
#define BLOCK_SIZE 512

precision highp float;
precision highp int;
precision highp sampler2DArray;

in vec2 frag_tex;
flat in int frag_instanceId;

layout(std140) uniform MaterialProperties
{
    vec2 scaling[BLOCK_SIZE];
    vec2 uvscale[BLOCK_SIZE];
    vec2 offset[BLOCK_SIZE];
    int source[BLOCK_SIZE];
    uint layer[BLOCK_SIZE];
    float bias[BLOCK_SIZE];
//    uint pad;
} mats;

uniform sampler2DArray bc1_tex;
uniform sampler2DArray bc3_tex;
uniform sampler2DArray bc5_tex;

out vec4 out_color;

void main()
{
    vec2 tex_ = frag_tex * mats.uvscale[frag_instanceId];
    tex_ = tex_ - floor(tex_);

    vec2 sample_pos = tex_ * mats.scaling[frag_instanceId]
            + vec2(mats.offset[frag_instanceId]);
    uint layer = mats.layer[frag_instanceId];

    float bias = mats.bias[frag_instanceId];

    if(mats.source[frag_instanceId] == 0)
        out_color = texture(bc1_tex, vec3(sample_pos, layer), bias);
    else if(mats.source[frag_instanceId] == 1)
        out_color = texture(bc3_tex, vec3(sample_pos, layer), bias);
    else if(mats.source[frag_instanceId] == 2)
        out_color = texture(bc5_tex, vec3(sample_pos, layer), bias);

//    out_color.rgb *= 0.01;
//    out_color.rgb += vec3(frag_tex.xy, float(layer) / 30.0);

    if(out_color.a < 0.5)
        discard;

//    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 0.4));
}
