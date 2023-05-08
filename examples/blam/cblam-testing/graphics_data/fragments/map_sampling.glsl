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
    int reflection;
    int meta1;
    int meta2;
};

struct MaterialData
{
    int id;
    int flags;
    vec2 input1;
    vec4 input2;
    vec4 input3;
    vec4 input4;

    vec4 input5;
    vec4 input6;
    vec4 input7;
    vec4 input8;
    vec4 input9;
    vec4 input10;
    vec4 input11;
    vec4 input12;
};

struct Material
{
    Map          maps[5];
    Lightmap     lightmap;
    MaterialData material;
};

layout(binding = 1, std140) buffer MaterialProperties
{
    Material instance[];
} mats;

vec4 get_map(in uint map_id, in int layer, in sampler2DArray sampler, in vec2 tex_coord, in int instance)
{
    if(layer == -1)
        return vec4(1.0);

    vec2 scale   = mats.instance[instance].maps[map_id].atlas_scale;
    vec2 uvscale = mats.instance[instance].maps[map_id].uv_scale;
    vec2 offset  = mats.instance[instance].maps[map_id].atlas_offset;

//    vec2 tc = (tex_coord - floor(tex_coord)) * uvscale;
    vec2 tc = tex_coord * uvscale;
    tc = tc - floor(tc);

    return texture(sampler, vec3(tc * scale + offset, layer & 0xFFFF));
}

#if USE_REFLECTIONS == 1
vec4 get_cube_map(
#if USE_ARRAY_CUBEMAP == 1
    in samplerCubeArray sampler,
#else
    in samplerCube sampler,
#endif
    in vec3 tex_coord,
    in int instance)
{
#if USE_ARRAY_CUBEMAP == 1
    int layer = mats.instance[instance].lightmap.reflection;
    return texture(sampler, vec4(tex_coord, layer & 0xFFFF));
#else
    return texture(sampler, vec3(tex_coord));
#endif
}
#endif

uint get_material_id(in uint instance)
{
    return mats.instance[instance].material.id;
}

#if USE_LIGHTMAPS == 1
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
