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
    vec2 padding;
};

struct Material
{
    Map      maps[4];
    Lightmap lightmap;
};
