#version 320 es

#extension GL_EXT_shader_io_blocks : enable

precision highp float;
precision highp int;
precision highp sampler2DArray;

in FragData {
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

uniform sampler2DArray base;
uniform sampler2DArray primary;
uniform sampler2DArray secondary;
uniform sampler2DArray micro;
uniform sampler2DArray lightmaps;

uniform float render_distance;

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[400];
} mats;

out vec4 out_color;

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

    out_color.rgb =
            color.rgb *
//            (
//                (primary.rgb * map_mix) +
//                (secondary.rgb * (1.0 - map_mix))
//            ) *
            micro_col.rgb *
            lightmap.rgb
            ;

    out_color.rgb = pow(out_color.rgb, vec3(1.0 / 1.5));

//    out_color.rgb *= 0.01;
//    out_color.rgb = lightmap.rgb;

//    out_color.rgb = abs(frag.normal);

//    out_color.rgb = texture(primary, vec3(frag.tex, frag.instanceId)).rgb;
    out_color.a = 1.0;
}
