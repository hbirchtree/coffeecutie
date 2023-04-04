#version 460 core

layout(location = 0) in FragData {
    vec3 world_pos;
    vec2 tex;
    vec3 normal;
    vec3 binormal;
    vec3 tangent;
    vec2 light_tex;
    flat int instanceId;
} frag;

struct Map
{
    vec2 atlas_scale;
    vec2 atlas_offset;
    vec2 uv_scale;
    int  layer;
    float bias;
};

struct Lightmap
{
    vec2 atlas_scale;
    vec2 atlas_offset;
    int layer;
    vec2 padding;
};

struct Material
{
    Map      maps[4];
    Lightmap lightmap;
};

layout(location = 10, binding = 0) uniform sampler2DArray base;
layout(location = 11, binding = 1) uniform sampler2DArray primary;
layout(location = 12, binding = 2) uniform sampler2DArray secondary;
layout(location = 13, binding = 3) uniform sampler2DArray micro;
layout(location = 14, binding = 4) uniform sampler2DArray lightmaps;
layout(location = 15) uniform float render_distance;

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

layout(location = 0) out vec4 final_color;

const uint base_map_id  = 0U;
const uint micro_map_id = 1U;
const uint prim_map_id  = 2U;
const uint seco_map_id  = 3U;

vec4 get_map(uint map_id, sampler2DArray sampler)
{
    int layer    = mats.instance[frag.instanceId].maps[map_id].layer;

    if(layer == -1)
        return vec4(1.0);

    vec2 scale   = mats.instance[frag.instanceId].maps[map_id].atlas_scale;
    vec2 uvscale = mats.instance[frag.instanceId].maps[map_id].uv_scale;
    vec2 offset  = mats.instance[frag.instanceId].maps[map_id].atlas_offset;

    vec2 tc = frag.tex * uvscale;
    tc = tc - floor(tc);

    return texture(sampler, vec3(tc * scale + offset, layer));
}

vec4 get_light()
{
    vec2 light_scale = mats.instance[frag.instanceId].lightmap.atlas_scale;
    vec2 light_offset = mats.instance[frag.instanceId].lightmap.atlas_offset;
    int light_layer = mats.instance[frag.instanceId].lightmap.layer;

    return texture(lightmaps, vec3(
                frag.light_tex * light_scale + light_offset,
                light_layer), -100.0);
}

void main()
{
//    if(length(frag.world_pos - vec3(0)) > render_distance)
//        discard;

    vec4 color     = get_map(base_map_id, base);
    vec4 primary   = get_map(prim_map_id, primary);
    vec4 secondary = get_map(seco_map_id, secondary);
    vec4 micro_col = get_map(micro_map_id, micro);

    vec4 lightmap = get_light();

    float map_mix = color.a;

    vec3 out_color =
            color.rgb *
            (
                (primary.rgb * map_mix) +
                (secondary.rgb * (1.0 - map_mix))
            ) *
            micro_col.rgb *
            lightmap.rgb
            ;

//    out_color.rgb *= 0.01;
//    out_color.rgb = lightmap.rgb;

//    out_color.rgb = abs(frag.normal);

//    out_color.rgb = texture(primary, vec3(frag.tex, frag.instanceId)).rgb;

    out_color.rgb = frag.tex.xyy;
    final_color = vec4(out_color.rgb, 1.0);
}
