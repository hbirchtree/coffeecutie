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

/* Mirrors materials::transparent_data::stage_t (64 bytes each) */
struct TransparentStage
{
    uint  color_in;  /* 4×(5-bit input + 3-bit mapping) */
    uint  alpha_in;  /* same, blam::shader::color_input enum  */
    uint  outputs;   /* color_out[0..16] | alpha_out[17..31]  */
    uint  flags;
    vec4  color0;    /* constant_color0 (CPU-animated; lower bound when
                        a_out_controls_color0_anim) */
    vec4  color0_up; /* constant_color0 upper bound for per-pixel anim */
    vec4  color1;    /* constant_color1 (static tint) */
};

/* Mirrors materials::transparent_data (464 bytes) */
struct TransparentData
{
    uint           num_stages;
    uint           blend_mode; /* chicago::framebuffer_blending */
    uint           pad0, pad1;
    TransparentStage stages[7];
};

layout(binding = 1, std140) uniform MaterialProperties
{
    Material instance[128];
} mats;

layout(binding = 4, std140) uniform TransparentProperties
{
    TransparentData instance[128];
} tr;

vec4 get_map(in uint map_id, in int layer, in sampler2DArray sampler, in vec2 tex_coord, in Material mat)
{
    if(layer == -1)
        return vec4(1.0);

    vec2 scale   = mat.maps[map_id].atlas_scale;
    vec2 uvscale = mat.maps[map_id].uv_scale;
    vec2 offset  = mat.maps[map_id].atlas_offset;

    vec2 uv = tex_coord * uvscale;
    vec2 tc = (uv - floor(uv)) * scale + offset;

    vec2 grad = scale * exp2(mat.maps[map_id].bias);
    return textureGrad(sampler, vec3(tc, layer & 0xFFFF),
                       dFdx(uv) * grad, dFdy(uv) * grad);
}

#if USE_REFLECTIONS == 1
vec4 get_cube_map(
#if USE_ARRAY_CUBEMAP == 1
    in samplerCubeArray sampler,
#else
    in samplerCube sampler,
#endif
    in vec3 tex_coord,
    in Material mat)
{
#if USE_ARRAY_CUBEMAP == 1
    int tex_id = mat.lightmap.reflection;
    // Explicit mip level to not fuzz with in-face texcoords
    float lod = float((tex_id >> 16) & 0xFF);
    return textureLod(sampler, vec4(tex_coord, tex_id & 0xFFFF), lod);
#else
    return texture(sampler, vec3(tex_coord));
#endif
}
#endif

uint get_material_id(in Material mat)
{
    return mat.material.id;
}

#if USE_LIGHTMAPS == 1
layout(location = 14, binding = 4) uniform sampler2DArray lightmaps;

#ifndef LIGHTMAP_FORMAT_AWARE
vec4 get_light(in Material mat, in vec2 light_tex)
{
    vec2 light_scale = mat.lightmap.atlas_scale;
    vec2 light_offset = mat.lightmap.atlas_offset;
    int light_layer = mat.lightmap.layer & 0xFFFF;

    return texture(lightmaps, vec3(
                light_tex * light_scale + light_offset,
                light_layer), -100.0);
}
#endif
#endif
