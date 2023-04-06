#version 460 core

#extension GL_GOOGLE_include_directive : enable

#include "fragments/map_sampling.glsl"

layout(location = 0) in FragData {
    vec3 world_pos;
    vec2 tex;
    vec3 normal;
    vec3 binormal;
    vec3 tangent;
    vec2 light_tex;
    flat int instanceId;
} frag;

layout(location = 16, binding = 0) uniform sampler2DArray bc1_tex;
layout(location = 17, binding = 1) uniform sampler2DArray bc3_tex;
layout(location = 18, binding = 2) uniform sampler2DArray bc5_tex;
layout(location = 13, binding = 3) uniform sampler2DArray source_rgb565;

layout(location = 0) out vec4 out_color;

void main()
{
//    out_color = vec4(get_light(frag.instanceId, frag.light_tex).rgb, 1.0);

//    vec2 tex_ = frag.tex - floor(frag.tex);

//    vec2 sample_pos = tex_ * mats.instance[frag.instanceId].scaling
//            + vec2(mats.instance[frag.instanceId].offset);

//    uint layer = mats.instance[frag.instanceId].layer;

//    if(mats.instance[frag.instanceId].source == 0)
//        out_color = texture(bc1_tex, vec3(sample_pos, layer));
//    else if(mats.instance[frag.instanceId].source == 1)
//        out_color = texture(bc3_tex, vec3(sample_pos, layer));
//    else if(mats.instance[frag.instanceId].source == 2)
//        out_color = texture(bc5_tex, vec3(sample_pos, layer));

    out_color = vec4(frag.tex.xyy, 1.0);
    //    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 0.8));
}
