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
    int material_id;
    int extension;
    int padding;
};

struct Material
{
    Map      maps[4];
    Lightmap lightmap;
};

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

vec4 get_map(in uint map_id, in sampler2DArray sampler, in vec2 tex_coord, in int instance)
{
    int layer = mats.instance[instance].maps[map_id].layer;

    if(layer == -1)
        return vec4(1.0);

    vec2 scale   = mats.instance[instance].maps[map_id].atlas_scale;
    vec2 uvscale = mats.instance[instance].maps[map_id].uv_scale;
    vec2 offset  = mats.instance[instance].maps[map_id].atlas_offset;

    vec2 tc = tex_coord * uvscale;
    tc = tc - floor(tc);

    return texture(sampler, vec3(tc * scale + offset, layer & 0xFFFF));
}

uint get_material_id(in uint instance)
{
    return mats.instance[instance].lightmap.material_id;
}

#if USE_BSP_SHADERS == 1
layout(location = 14, binding = 4) uniform sampler2DArray lightmaps;

vec4 get_light(in uint instance, in vec2 light_tex)
{
    vec2 light_scale = mats.instance[instance].lightmap.atlas_scale;
    vec2 light_offset = mats.instance[instance].lightmap.atlas_offset;
    int light_layer = mats.instance[instance].lightmap.layer & 0xFFFF;

    return texture(lightmaps, vec3(
                light_tex * light_scale + light_offset,
                light_layer), -100.0);
}
#endif
